#include "EventHandler.hpp"

#include "Mpc.hpp"

#include "audiomidi/MidiOutput.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"

#include "audiomidi/AudioMidiServices.hpp"

#include "hardware/Hardware.hpp"
#include "sequencer/Bus.hpp"
#include "engine/DrumNoteEventHandler.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"

#include "sequencer/FrameSeq.hpp"
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

#include "eventregistry/EventRegistry.hpp"

using namespace mpc::audiomidi;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::engine;

EventHandler::EventHandler(mpc::Mpc &mpcToUse) : mpc(mpcToUse)
{
    transposeCache.reserve(512);
}

void EventHandler::handleFinalizedDrumNoteOnEvent(
    std::shared_ptr<NoteOnEvent> noteOnEvent,
    const std::shared_ptr<DrumBus> drumBus,
    Track *track)
{
    auto sampler = mpc.getSampler();
    const auto program = sampler->getProgram(drumBus->getProgram());
    const auto note = noteOnEvent->getNote();
    const int programPadIndex = program->getPadIndexFromNote(note);

    const uint64_t noteEventIdToUse = noteEventId++;
    const auto velocityWithTrackVelocityRatioApplied = static_cast<int>(
        noteOnEvent->getVelocity() * (track->getVelocityRatio() * 0.01f));

    const auto velocityToUse =
        std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);
    const auto noteParameters =
        program->getNoteParameters(note);

    const auto sound = sampler->getSound(noteParameters->getSoundIndex());

    const auto voiceOverlapMode =
        (sound && sound->isLoopEnabled())
            ? VoiceOverlapMode::NOTE_OFF
            : noteParameters->getVoiceOverlapMode();

    const bool isSliderNote =
        program && program->getSlider()->getNote() == note;

    if (mpc.clientEventController->isAfterEnabled() && isSliderNote && mpc.getSequencer()->isOverdubbing())
    {
        auto programSlider = program->getSlider();

        int variationTypeToUse = noteOnEvent->getVariationType();
        int variationValueToUse = noteOnEvent->getVariationValue();

        Util::SliderNoteVariationContext sliderNoteVariationContext{
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

        const auto sliderValue =
            mpc.getHardware()->getSlider()->getValue();

        auto [type, value] = Util::getSliderNoteVariationTypeAndValue(
            sliderNoteVariationContext);
        variationTypeToUse = type;
        variationValueToUse = value;

        noteOnEvent->setVariationType(type);
        noteOnEvent->setVariationValue(variationValueToUse);
    }

    const auto frameSeq = mpc.getSequencer()->getFrameSequencer();
    const auto eventFrameOffsetInBuffer = frameSeq->getEventFrameOffset();
    const auto durationTicks = *noteOnEvent->getDuration();
    const auto audioMidiServices = mpc.getAudioMidiServices();
    const auto audioServer = audioMidiServices->getAudioServer();
    const auto durationFrames = SeqUtil::ticksToFrames(
        durationTicks, mpc.getSequencer()->getTempo(),
        audioServer->getSampleRate());

    auto ctx = engine::DrumNoteEventContextBuilder::buildDrumNoteOnContext(
        noteEventIdToUse, drumBus, mpc.getSampler(),
        mpc.getAudioMidiServices()->getMixer(),
        mpc.screens->get<ScreenId::MixerSetupScreen>(),
        &mpc.getAudioMidiServices()->getVoices(),
        mpc.getAudioMidiServices()->getMixerConnections(), note,
        velocityToUse, noteOnEvent->getVariationType(),
        noteOnEvent->getVariationValue(), eventFrameOffsetInBuffer, true,
        noteOnEvent->getTick(),
        voiceOverlapMode == VoiceOverlapMode::NOTE_OFF ? durationFrames
                                                       : -1);

    DrumNoteEventHandler::noteOn(ctx);

    auto currentScreen = mpc.getLayeredScreen()->getCurrentScreen();

    mpc.eventRegistry->registerNoteOn(
        eventregistry::Source::Sequence, currentScreen, ctx.drum,
        note, noteOnEvent->getVelocity(), track,
        std::nullopt, program, [](void *) {});

    if (programPadIndex != -1)
    {
        mpc.eventRegistry->registerProgramPadPress(
            eventregistry::Source::Sequence, currentScreen, ctx.drum, program,
            programPadIndex, noteOnEvent->getVelocity(), track, std::nullopt);
    }

    const auto noteOffCtx = DrumNoteEventContextBuilder::buildDrumNoteOffContext(
        noteEventIdToUse, drumBus, &mpc.getAudioMidiServices()->getVoices(),
        note, noteOnEvent->getTick());

    auto noteOffEventFn = [bus = ctx.drum, note = noteOnEvent->getNote(),
                             track, eventRegistry = mpc.eventRegistry,
                             program, programPadIndex, noteOffCtx]
    {
        eventRegistry->registerNoteOff(eventregistry::Source::Sequence, bus,
                                       note, track, std::nullopt,
                                       [](void *) {});

        if (programPadIndex != -1)
        {
            eventRegistry->registerProgramPadRelease(
                eventregistry::Source::Sequence, bus, program, programPadIndex,
                track, std::nullopt, [](void *) {});
        }

        DrumNoteEventHandler::noteOff(noteOffCtx);
    };

    frameSeq->enqueueEventAfterNFrames(
        noteOffEventFn, durationFrames + eventFrameOffsetInBuffer);
}

void EventHandler::handleFinalizedEvent(const std::shared_ptr<Event> &event,
                                        Track *const track)
{
    if (mpc.getSequencer()->isCountingIn())
    {
        return;
    }

    if (std::dynamic_pointer_cast<TempoChangeEvent>(event))
    {
        return;
    }

    if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);
        noteOnEvent)
    {
        assert(noteOnEvent->getDuration().has_value() &&
               *noteOnEvent->getDuration() >= 0);

        if (auto drumBus = mpc.getSequencer()->getBus<DrumBus>(track->getBus());
            drumBus)
        {
            if (isDrumNote(noteOnEvent->getNote()))
            {
                handleFinalizedDrumNoteOnEvent(noteOnEvent, drumBus, track);
            }
        }

        handleNoteEventMidiOut(event, track, track->getDeviceIndex(),
                               track->getVelocityRatio());

        auto midiNoteOffEventFn =
            [this, noteOffEvent = noteOnEvent->getNoteOff(), track,
             trackDeviceIndex = track->getDeviceIndex()]
        {
            handleNoteEventMidiOut(noteOffEvent, track, trackDeviceIndex,
                                   std::nullopt);
        };

        const auto frameSeq = mpc.getSequencer()->getFrameSequencer();
        const auto eventFrameOffsetInBuffer = frameSeq->getEventFrameOffset();
        const auto durationTicks = *noteOnEvent->getDuration();
        const auto audioMidiServices = mpc.getAudioMidiServices();
        const auto audioServer = audioMidiServices->getAudioServer();
        const auto durationFrames = SeqUtil::ticksToFrames(
            durationTicks, mpc.getSequencer()->getTempo(),
            audioServer->getSampleRate());

        frameSeq->enqueueEventAfterNFrames(
            midiNoteOffEventFn, durationFrames + eventFrameOffsetInBuffer);
    }
    else if (auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
             mixerEvent != nullptr)
    {
        auto pad = mixerEvent->getPad();
        auto sampler = mpc.getSampler();
        auto drumBus = mpc.getSequencer()->getBus<DrumBus>(track->getBus());

        assert(drumBus);

        auto program = sampler->getProgram(drumBus->getProgram());

        auto mixerSetupScreen = mpc.screens->get<ScreenId::MixerSetupScreen>();

        auto mixer = mixerSetupScreen->isStereoMixSourceDrum()
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
    const std::optional<int> drumIndex)
{
    assert(noteOnEvent);
    assert(!noteOnEvent->isFinalized());

    const uint64_t noteEventIdToUse = noteEventId++;

    if (drumIndex.has_value() && isDrumNote(noteOnEvent->getNote()))
    {
        auto drumBus = mpc.getSequencer()->getDrumBus(*drumIndex);
        assert(drumBus);
        const auto program =
            mpc.getSampler()->getProgram(drumBus->getProgram());
        const auto note = noteOnEvent->getNote();

        const auto velocityWithTrackVelocityRatioApplied =
            static_cast<int>(noteOnEvent->getVelocity() *
                             (trackVelocityRatio.value_or(100) * 0.01f));

        const auto velocityToUse =
            std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        auto ctx = engine::DrumNoteEventContextBuilder::buildDrumNoteOnContext(
            0, drumBus, mpc.getSampler(),
            mpc.getAudioMidiServices()->getMixer(),
            mpc.screens->get<ScreenId::MixerSetupScreen>(),
            &mpc.getAudioMidiServices()->getVoices(),
            mpc.getAudioMidiServices()->getMixerConnections(), note,
            velocityToUse, noteOnEvent->getVariationType(),
            noteOnEvent->getVariationValue(), 0, true, -1, -1);

        DrumNoteEventHandler::noteOn(ctx);
    }

    if (track != nullptr && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOnEvent, track, *trackDevice,
                               trackVelocityRatio);
    }
}

// Input from physical pad releases
void EventHandler::handleNoteOffFromUnfinalizedNoteOn(
    const std::shared_ptr<NoteOffEvent> &noteOffEvent, Track *track,
    const std::optional<int> trackDevice, const std::optional<int> drumIndex)
{
    assert(noteOffEvent);

    if (drumIndex.has_value() && isDrumNote(noteOffEvent->getNote()))
    {
        auto drumBus = mpc.getSequencer()->getDrumBus(*drumIndex);

        assert(drumBus);

        const auto program =
            mpc.getSampler()->getProgram(drumBus->getProgram());
        const auto note = noteOffEvent->getNote();

        auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOffContext(
            0, drumBus, &mpc.getAudioMidiServices()->getVoices(),
            noteOffEvent->getNote(), -1);

        DrumNoteEventHandler::noteOff(ctx);
    }

    if (track != nullptr && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOffEvent, track, *trackDevice, std::nullopt);
    }
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
void EventHandler::handleNoteEventMidiOut(
    const std::shared_ptr<Event> &event, Track *track, const int trackDevice,
    const std::optional<int> trackVelocityRatio)
{
    assert(std::dynamic_pointer_cast<NoteOnEvent>(event) ||
           std::dynamic_pointer_cast<NoteOffEvent>(event));

    assert(trackVelocityRatio.has_value() ||
           std::dynamic_pointer_cast<NoteOffEvent>(event));

    const auto isRealTime =
        mpc.getAudioMidiServices()->getAudioServer()->isRealTime();

    if (!isRealTime || trackDevice == 0)
    {
        return;
    }

    // Derives a 0-based MIDI channel.
    const int midiChannel = (trackDevice - 1) % 16;

    int transposeAmount = 0;

    if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event))
    {
        auto transScreen = mpc.screens->get<ScreenId::TransScreen>();

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

        const auto audioMidiServices = mpc.getAudioMidiServices();
        const auto frameSeq = mpc.getSequencer()->getFrameSequencer();
        const auto audioServer = audioMidiServices->getAudioServer();

        if (noteOnEvent->isFinalized())
        {
            frameSeq->enqueueEventAfterNFrames(
                [this, noteOnEvent, track, trackDevice]
                {
                    handleNoteEventMidiOut(noteOnEvent->getNoteOff(), track,
                                           trackDevice, std::nullopt);
                },
                SeqUtil::ticksToFrames(*noteOnEvent->getDuration(),
                                       mpc.getSequencer()->getTempo(),
                                       audioServer->getSampleRate()));
        }
    }
    else if (auto noteOffEvent = std::dynamic_pointer_cast<NoteOffEvent>(event))
    {
        auto candidate = transposeCache.find(noteOffEvent);

        if (candidate != end(transposeCache) && track->getIndex() < 64)
        {
            transposeAmount = candidate->second;
            transposeCache.erase(candidate);
        }

        // msg = noteOffEvent->createShortMessage(midiChannel, transposeAmount);
    }

    const auto directToDiskRecorderScreen =
        mpc.screens->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (!(mpc.getAudioMidiServices()->isBouncing() &&
          directToDiskRecorderScreen->offline) &&
        trackDevice != 0)
    {
        // msg->bufferPos = mpc.getAudioMidiServices()
        //->getFrameSequencer()
        //->getEventFrameOffset();
        // mpc.getMidiOutput()->enqueueMessageOutputA(msg);
    }

    // For the MIDI output monitor screen
    notifyObservers(midiChannel < 16 ? "a" : "b" + std::to_string(midiChannel));
}
