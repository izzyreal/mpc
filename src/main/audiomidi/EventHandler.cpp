#include "EventHandler.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"

#include "controller/ClientEventController.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"
#include "engine/EngineHost.hpp"
#include "engine/SequencerPlaybackEngine.hpp"

#include "hardware/Hardware.hpp"
#include "sequencer/Bus.hpp"
#include "engine/DrumNoteEventHandler.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"

#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"

#include "Util.hpp"

#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "lcdgui/screens/TransScreen.hpp"

#include <cassert>
#include <memory>

#include "eventregistry/PerformanceManager.hpp"
#include "lcdgui/ScreenNames.hpp"

using namespace mpc::audiomidi;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::engine;

EventHandler::EventHandler(Mpc &mpc) : mpc(mpc)
{
    transposeCache.reserve(512);
}

void EventHandler::handleFinalizedDrumNoteOnEvent(
    const std::shared_ptr<NoteOnEvent> &noteOnEvent,
    const std::shared_ptr<DrumBus> &drumBus, const Track *track)
{
    const auto sampler = mpc.getSampler();
    const auto programIndex = drumBus->getProgram();
    const auto program = sampler->getProgram(drumBus->getProgram());
    const auto note = noteOnEvent->getNote();
    const ProgramPadIndex programPadIndex =
        program->getPadIndexFromNote(DrumNoteNumber(note));

    const uint64_t noteEventIdToUse = noteEventId++;
    const auto velocityWithTrackVelocityRatioApplied = static_cast<int>(
        noteOnEvent->getVelocity() * (track->getVelocityRatio() * 0.01f));

    const auto velocityToUse =
        std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);
    const auto noteParameters = program->getNoteParameters(note);

    const auto sound = sampler->getSound(noteParameters->getSoundIndex());

    const auto voiceOverlapMode = sound && sound->isLoopEnabled()
                                      ? VoiceOverlapMode::NOTE_OFF
                                      : noteParameters->getVoiceOverlapMode();

    const bool isSliderNote =
        program && program->getSlider()->getNote() == note;

    if (mpc.clientEventController->isAfterEnabled() && isSliderNote &&
        mpc.getSequencer()->getTransport()->isOverdubbing())
    {
        const auto programSlider = program->getSlider();

        const Util::SliderNoteVariationContext sliderNoteVariationContext{
            mpc.getHardware()->getSlider()->getValueAs<int>(),
            programSlider->getNote(),
            programSlider->getParameter(),
            programSlider->getTuneLowRange(),
            programSlider->getTuneHighRange(),
            programSlider->getDecayLowRange(),
            programSlider->getDecayHighRange(),
            programSlider->getAttackLowRange(),
            programSlider->getAttackHighRange(),
            programSlider->getFilterLowRange(),
            programSlider->getFilterHighRange()};

        auto [type, value] = Util::getSliderNoteVariationTypeAndValue(
            sliderNoteVariationContext);

        noteOnEvent->setVariationType(type);
        noteOnEvent->setVariationValue(value);
    }

    const auto engineHost = mpc.getEngineHost();
    const auto sequencerPlaybackEngine =
        engineHost->getSequencerPlaybackEngine();
    const auto eventFrameOffsetInBuffer =
        sequencerPlaybackEngine->getEventFrameOffset();
    const auto durationTicks = *noteOnEvent->getDuration();
    const auto audioServer = engineHost->getAudioServer();
    const auto durationFrames = SeqUtil::ticksToFrames(
        durationTicks, mpc.getSequencer()->getTransport()->getTempo(),
        audioServer->getSampleRate());

    const auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
        noteEventIdToUse, drumBus, mpc.getSampler(),
        mpc.getEngineHost()->getMixer(),
        mpc.screens->get<ScreenId::MixerSetupScreen>(),
        &mpc.getEngineHost()->getVoices(),
        mpc.getEngineHost()->getMixerConnections(), note, velocityToUse,
        noteOnEvent->getVariationType(), noteOnEvent->getVariationValue(),
        eventFrameOffsetInBuffer, true, noteOnEvent->getTick(),
        voiceOverlapMode == VoiceOverlapMode::NOTE_OFF ? durationFrames : -1);

    DrumNoteEventHandler::noteOn(ctx);

    const auto screenId = mpc.getLayeredScreen()->getCurrentScreenId();

    mpc.performanceManager->registerNoteOn(
        eventregistry::PerformanceEventSource::Sequence, std::nullopt, screenId,
        track->getIndex(), ctx.drum->busType, note, noteOnEvent->getVelocity(),
        programIndex, [](void *) {});

    if (programPadIndex != -1)
    {
        mpc.performanceManager->registerProgramPadPress(
            eventregistry::PerformanceEventSource::Sequence, std::nullopt, screenId,
            track->getIndex(), ctx.drum->busType, programPadIndex,
            noteOnEvent->getVelocity(), programIndex, NoPhysicalPadIndex);
    }

    const auto noteOffCtx =
        DrumNoteEventContextBuilder::buildDrumNoteOffContext(
            noteEventIdToUse, drumBus, &mpc.getEngineHost()->getVoices(), note,
            noteOnEvent->getTick());

    auto noteOffEventFn = [note = noteOnEvent->getNote(), programIndex,
                           performanceManager = mpc.performanceManager, programPadIndex,
                           noteOffCtx]
    {
        constexpr std::optional<MidiChannel> noMidiChannel = std::nullopt;

        performanceManager->registerNoteOff(eventregistry::PerformanceEventSource::Sequence, note,
                                       noMidiChannel, [](void *) {});

        if (programPadIndex != -1)
        {
            performanceManager->registerProgramPadRelease(
                eventregistry::PerformanceEventSource::Sequence, programPadIndex, programIndex,
                [](void *) {});
        }

        DrumNoteEventHandler::noteOff(noteOffCtx);
    };

    sequencerPlaybackEngine->enqueueEventAfterNFrames(
        noteOffEventFn, durationFrames + eventFrameOffsetInBuffer);
}

void EventHandler::handleFinalizedEvent(const std::shared_ptr<Event> &event,
                                        Track *const track)
{
    if (mpc.getSequencer()->getTransport()->isCountingIn())
    {
        return;
    }

    if (std::dynamic_pointer_cast<TempoChangeEvent>(event))
    {
        return;
    }

    if (const auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);
        noteOnEvent)
    {
        assert(noteOnEvent->getDuration().has_value() &&
               *noteOnEvent->getDuration() >= 0);

        if (const auto drumBus =
                mpc.getSequencer()->getBus<DrumBus>(track->getBusType());
            drumBus)
        {
            if (isDrumNote(noteOnEvent->getNote()))
            {
                handleFinalizedDrumNoteOnEvent(noteOnEvent, drumBus, track);
            }
        }

        // handleNoteEventMidiOut(event, track, track->getDeviceIndex(),
        //                        track->getVelocityRatio());

        auto midiNoteOffEventFn =
            [this, noteOffEvent = noteOnEvent->getNoteOff(), track,
             trackDeviceIndex = track->getDeviceIndex()]
        {
            // handleNoteEventMidiOut(noteOffEvent, track, trackDeviceIndex,
            //                        std::nullopt);
        };

        const auto engineHost = mpc.getEngineHost();
        const auto sequencerPlaybackEngine =
            engineHost->getSequencerPlaybackEngine();
        const auto eventFrameOffsetInBuffer =
            sequencerPlaybackEngine->getEventFrameOffset();
        const auto durationTicks = *noteOnEvent->getDuration();
        const auto audioServer = engineHost->getAudioServer();
        const auto durationFrames = SeqUtil::ticksToFrames(
            durationTicks, mpc.getSequencer()->getTransport()->getTempo(),
            audioServer->getSampleRate());

        sequencerPlaybackEngine->enqueueEventAfterNFrames(
            midiNoteOffEventFn, durationFrames + eventFrameOffsetInBuffer);
    }
    else if (const auto mixerEvent =
                 std::dynamic_pointer_cast<MixerEvent>(event);
             mixerEvent != nullptr)
    {
        const auto pad = mixerEvent->getPad();
        const auto sampler = mpc.getSampler();
        const auto drumBus =
            mpc.getSequencer()->getBus<DrumBus>(track->getBusType());

        assert(drumBus);

        const auto program = sampler->getProgram(drumBus->getProgram());

        const auto mixerSetupScreen =
            mpc.screens->get<ScreenId::MixerSetupScreen>();

        const auto mixer = mixerSetupScreen->isStereoMixSourceDrum()
                               ? drumBus->getStereoMixerChannels()[pad]
                               : program->getStereoMixerChannel(pad);

        if (mixerEvent->getParameter() == 0)
        {
            mixer->setLevel(mixerEvent->getValue());
        }
        else if (mixerEvent->getParameter() == 1)
        {
            mixer->setPanning(mixerEvent->getValue());
        }
    }
}

void EventHandler::handleUnfinalizedNoteOn(
    const std::shared_ptr<NoteOnEvent> &noteOnEvent, Track *track,
    const std::optional<int> trackDevice,
    const std::optional<int> trackVelocityRatio,
    const std::optional<BusType> drumBusType) const
{
    assert(noteOnEvent);
    assert(!noteOnEvent->isFinalized());

    if (drumBusType.has_value() && isDrumNote(noteOnEvent->getNote()))
    {
        const auto drumBus = mpc.getSequencer()->getDrumBus(*drumBusType);
        assert(drumBus);
        const auto program =
            mpc.getSampler()->getProgram(drumBus->getProgram());
        const auto note = noteOnEvent->getNote();

        const auto velocityWithTrackVelocityRatioApplied =
            static_cast<int>(noteOnEvent->getVelocity() *
                             (trackVelocityRatio.value_or(100) * 0.01f));

        const auto velocityToUse =
            std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        const auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
            0, drumBus, mpc.getSampler(), mpc.getEngineHost()->getMixer(),
            mpc.screens->get<ScreenId::MixerSetupScreen>(),
            &mpc.getEngineHost()->getVoices(),
            mpc.getEngineHost()->getMixerConnections(), note, velocityToUse,
            noteOnEvent->getVariationType(), noteOnEvent->getVariationValue(),
            0, true, -1, -1);

        DrumNoteEventHandler::noteOn(ctx);
    }

    /*
    if (track != nullptr && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOnEvent, track, *trackDevice,
                               trackVelocityRatio);
    }
*/
}

// Input from physical pad releases
void EventHandler::handleNoteOffFromUnfinalizedNoteOn(
    const std::shared_ptr<NoteOffEvent> &noteOffEvent, Track *track,
    const std::optional<int> trackDevice,
    const std::optional<DrumBusIndex> drumBusIndex) const
{
    assert(noteOffEvent);

    if (drumBusIndex.has_value() && isDrumNote(noteOffEvent->getNote()))
    {
        const auto drumBus = mpc.getSequencer()->getDrumBus(*drumBusIndex);

        assert(drumBus);

        const auto program =
            mpc.getSampler()->getProgram(drumBus->getProgram());
        const auto note = noteOffEvent->getNote();

        const auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOffContext(
            0, drumBus, &mpc.getEngineHost()->getVoices(), note, -1);

        DrumNoteEventHandler::noteOff(ctx);
    }

    /*
    if (track != nullptr && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOffEvent, track, *trackDevice, std::nullopt);
    }
*/
}

/**
 * Creates MIDI note-on and note-off events and queues them for emission to the
 * MIDI output port.
 *
 * The event must be a NoteOnEvent or NoteOffEvent produced by the sequencer or
 * by physical pad presses. We don't deal with MIDI thru here. That is done by
 * mpc::audiomidi::MidiInput.
 *
 * If the NoteOnEvent is finalized (i.e. already has a duration, such as a note
 * event in a sequence), its MIDI note-off counterpart is immediately scheduled
 * for emission after the appropriate number of audio frames has passsed.
 *
 * If the NoteOnEvent is not finalized (i.e. has no duration yet, such as those
 * generated by realtime pad presses), the caller is responsible for invoking
 * handleNoteEventMidiOut at the appropriate time.
 */
/*
void EventHandler::handleNoteEventMidiOut(
    const std::shared_ptr<Event> &event, Track *track, const int trackDevice,
    const std::optional<int> trackVelocityRatio)
{
    assert(std::dynamic_pointer_cast<NoteOnEvent>(event) ||
           std::dynamic_pointer_cast<NoteOffEvent>(event));

    assert(trackVelocityRatio.has_value() ||
           std::dynamic_pointer_cast<NoteOffEvent>(event));

    if (const auto isRealTime =
            mpc.getEngineHost()->getAudioServer()->isRealTime();
        !isRealTime || trackDevice == 0)
    {
        return;
    }

    // Derives a 0-based MIDI channel.
    const int midiChannel = (trackDevice - 1) % 16;

    int transposeAmount = 0;

    if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event))
    {
        const auto transScreen = mpc.screens->get<ScreenId::TransScreen>();

        if (transScreen->transposeAmount != 0 && track->getIndex() < 64 &&
            (transScreen->tr == -1 || transScreen->tr == track->getIndex()))
        {
            transposeAmount = transScreen->transposeAmount;
            transposeCache[noteOnEvent->getNoteOff()] = transposeAmount;
        }

        // msg = noteOnEvent->createShortMessage(midiChannel, transposeAmount);

        const auto velocityWithTrackVelocityRatioApplied = static_cast<int>(
            noteOnEvent->getVelocity() * (*trackVelocityRatio * 0.01f));

        const auto velocityToUse =
            std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        // msg->setMessage(msg->getStatus(), msg->getChannel(), msg->getData1(),
        //                 velocityToUse);

        const auto engineHost = mpc.getEngineHost();
        const auto sequencerPlaybackEngine =
            engineHost->getSequencerPlaybackEngine();
        const auto audioServer = engineHost->getAudioServer();

        if (noteOnEvent->isFinalized())
        {
            sequencerPlaybackEngine->enqueueEventAfterNFrames(
                [this, noteOnEvent, track, trackDevice]
                {
                    handleNoteEventMidiOut(noteOnEvent->getNoteOff(), track,
                                           trackDevice, std::nullopt);
                },
                SeqUtil::ticksToFrames(
                    *noteOnEvent->getDuration(),
                    mpc.getSequencer()->getTransport()->getTempo(),
                    audioServer->getSampleRate()));
        }
    }
    else if (const auto noteOffEvent =
                 std::dynamic_pointer_cast<NoteOffEvent>(event))
    {

        if (const auto candidate = transposeCache.find(noteOffEvent);
            candidate != end(transposeCache) && track->getIndex() < 64)
        {
            transposeAmount = candidate->second;
            transposeCache.erase(candidate);
        }

        // msg = noteOffEvent->createShortMessage(midiChannel, transposeAmount);
    }

    const auto directToDiskRecorderScreen =
        mpc.screens->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (!(mpc.getEngineHost()->isBouncing() &&
          directToDiskRecorderScreen->offline) &&
        trackDevice != 0)
    {
        // msg->bufferPos = mpc.getEngineHost()
        //->getSequencerPlaybackEngine()
        //->getEventFrameOffset();
        // mpc.getMidiOutput()->enqueueMessageOutputA(msg);
    }

    // For the MIDI output monitor screen
    notifyObservers(midiChannel < 16 ? "a" : "b" + std::to_string(midiChannel));
}
*/