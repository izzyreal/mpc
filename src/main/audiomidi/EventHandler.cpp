#include "EventHandler.hpp"

#include "audiomidi/MidiOutput.hpp"
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

#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"

#include "Util.hpp"
#include "client/event/SeqEventToMidiEventMapper.hpp"
#include "controller/ClientMidiEventController.hpp"

#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "lcdgui/screens/TransScreen.hpp"

#include <cassert>
#include <memory>

#include "engine/StereoMixer.hpp"
#include "performance/PerformanceManager.hpp"
#include "lcdgui/ScreenNames.hpp"

#include <nlohmann/detail/meta/call_std/end.hpp>

using namespace mpc::audiomidi;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::engine;
using namespace mpc::client::event;

EventHandler::EventHandler(Mpc &mpc) : mpc(mpc) {}

EventHandler::~EventHandler() {}

void EventHandler::handleFinalizedDrumNoteOnEvent(
    const EventData &noteOnEvent, const std::shared_ptr<DrumBus> &drumBus,
    const TrackIndex trackIndex, const int trackVelocityRatio)
{
    const auto sampler = mpc.getSampler();
    const auto programIndex = drumBus->getProgramIndex();
    const auto program = sampler->getProgram(drumBus->getProgramIndex());
    const auto note = noteOnEvent.noteNumber;
    const ProgramPadIndex programPadIndex =
        program->getPadIndexFromNote(DrumNoteNumber(note));

    const uint64_t noteEventIdToUse = noteEventId++;
    const auto velocityWithTrackVelocityRatioApplied =
        static_cast<int>(noteOnEvent.velocity * (trackVelocityRatio * 0.01f));

    const auto velocityToUse =
        std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);
    const auto noteParameters = program->getNoteParameters(note);

    const auto sound = sampler->getSound(noteParameters->getSoundIndex());

    const auto voiceOverlapMode = sound && sound->isLoopEnabled()
                                      ? VoiceOverlapMode::NOTE_OFF
                                      : noteParameters->getVoiceOverlapMode();

    const bool isSliderNote =
        program && program->getSlider()->getNote() == note;

    NoteVariationType noteVariationType = noteOnEvent.noteVariationType;
    NoteVariationValue noteVariationValue = noteOnEvent.noteVariationValue;

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

        noteVariationType = type;
        noteVariationValue = NoteVariationValue(value);
    }

    const auto engineHost = mpc.getEngineHost();
    const auto sequencerPlaybackEngine =
        engineHost->getSequencerPlaybackEngine();
    const auto eventFrameOffsetInBuffer =
        sequencerPlaybackEngine->getEventFrameOffset();
    const auto durationTicks = noteOnEvent.duration;
    const auto audioServer = engineHost->getAudioServer();
    const auto durationFrames = SeqUtil::ticksToFrames(
        durationTicks, mpc.getSequencer()->getTransport()->getTempo(),
        audioServer->getSampleRate());

    auto performanceDrum =
        mpc.getPerformanceManager().lock()->getSnapshot().getDrum(
            drumBus->getIndex());

    const auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
        noteEventIdToUse, performanceDrum, drumBus, mpc.getSampler(),
        mpc.getEngineHost()->getMixer(),
        mpc.screens->get<ScreenId::MixerSetupScreen>(),
        &mpc.getEngineHost()->getVoices(),
        mpc.getEngineHost()->getMixerConnections(), note, velocityToUse,
        noteVariationType, noteVariationValue, eventFrameOffsetInBuffer, true,
        noteOnEvent.tick,
        voiceOverlapMode == VoiceOverlapMode::NOTE_OFF ? durationFrames : -1);

    DrumNoteEventHandler::noteOn(ctx);

    const auto screenId = mpc.getLayeredScreen()->getCurrentScreenId();

    mpc.getPerformanceManager().lock()->registerNoteOn(
        performance::PerformanceEventSource::Sequence, std::nullopt, screenId,
        trackIndex, drumBusIndexToDrumBusType(ctx.drum.drumBusIndex), note,
        Velocity(velocityToUse), programIndex, [](void *) {});

    if (programPadIndex != -1)
    {
        mpc.getPerformanceManager().lock()->registerProgramPadPress(
            performance::PerformanceEventSource::Sequence, std::nullopt,
            screenId, trackIndex,
            drumBusIndexToDrumBusType(ctx.drum.drumBusIndex), programPadIndex,
            Velocity(velocityToUse), programIndex, NoPhysicalPadIndex);
    }

    const auto noteOffCtx =
        DrumNoteEventContextBuilder::buildDrumNoteOffContext(
            noteEventIdToUse, drumBus, &mpc.getEngineHost()->getVoices(), note,
            noteOnEvent.tick);

    concurrency::SamplePreciseTask task;

    task.f.set(
        [note, programIndex, performanceManager = mpc.getPerformanceManager(),
         programPadIndex, noteOffCtx](int)
        {
            constexpr std::optional<MidiChannel> noMidiChannel = std::nullopt;

            performanceManager.lock()->registerNoteOff(
                performance::PerformanceEventSource::Sequence, note,
                noMidiChannel, [](void *) {});

            if (programPadIndex != -1)
            {
                performanceManager.lock()->registerProgramPadRelease(
                    performance::PerformanceEventSource::Sequence,
                    programPadIndex, programIndex, [](void *) {});
            }

            DrumNoteEventHandler::noteOff(noteOffCtx);
        });

    task.nFrames =
        static_cast<int64_t>(durationFrames) + eventFrameOffsetInBuffer;

    mpc.getEngineHost()->postSamplePreciseTaskToAudioThread(task);
}

void EventHandler::handleFinalizedEvent(const EventData &event,
                                        const TrackIndex trackIndex,
                                        const int trackVelocityRatio,
                                        const BusType trackBusType,
                                        const int trackDeviceIndex)
{
    if (mpc.getSequencer()->getTransport()->isCountingIn())
    {
        return;
    }

    if (event.type == EventType::TempoChange)
    {
        return;
    }

    if (event.type == EventType::NoteOn)
    {
        assert(event.noteNumber != NoNoteNumber &&
               event.duration != NoDuration);

        if (const auto drumBus =
                mpc.getSequencer()->getBus<DrumBus>(trackBusType);
            drumBus)
        {
            if (isDrumNote(event.noteNumber))
            {
                handleFinalizedDrumNoteOnEvent(event, drumBus, trackIndex,
                                               trackVelocityRatio);
            }
        }

        const auto transScreen = mpc.screens->get<ScreenId::TransScreen>();

        std::optional<int> transposeAmount = std::nullopt;

        if (transScreen->getTransposeAmount() != 0 &&
            (transScreen->getTr() == AllTrackIndex ||
             transScreen->getTr() == trackIndex))
        {
            transposeAmount = transScreen->getTransposeAmount();
        }

        const auto sampleNumber = mpc.getEngineHost()
                                      ->getSequencerPlaybackEngine()
                                      ->getEventFrameOffset();

        handleNoteEventMidiOut(event, trackDeviceIndex, trackVelocityRatio,
                               transposeAmount, sampleNumber);

        const auto engineHost = mpc.getEngineHost();
        const auto sequencerPlaybackEngine =
            engineHost->getSequencerPlaybackEngine();
        const auto audioServer = engineHost->getAudioServer();

        EventData noteOff = event;
        noteOff.type = EventType::NoteOff;

        concurrency::SamplePreciseTask task;
        task.midiNoteOff = true;
        task.f.set(
            [this, noteOff, trackDeviceIndex,
             transposeAmount](const int noteOffSampleNumber)
            {
                handleNoteEventMidiOut(noteOff, trackDeviceIndex, std::nullopt,
                                       transposeAmount, noteOffSampleNumber);
            });

        task.nFrames = SeqUtil::ticksToFrames(
            event.duration, mpc.getSequencer()->getTransport()->getTempo(),
            audioServer->getSampleRate());

        mpc.getEngineHost()->postSamplePreciseTaskToAudioThread(task);
    }
    else if (event.type == EventType::Mixer)
    {
        const auto pad = event.mixerPad;
        const auto sampler = mpc.getSampler();
        const auto drumBus = mpc.getSequencer()->getBus<DrumBus>(trackBusType);

        assert(drumBus);

        const auto program = sampler->getProgram(drumBus->getProgramIndex());

        const auto mixerSetupScreen =
            mpc.screens->get<ScreenId::MixerSetupScreen>();

        const auto mixer = mixerSetupScreen->isStereoMixSourceDrum()
                               ? drumBus->getStereoMixerChannels()[pad]
                               : program->getStereoMixerChannel(pad);

        if (event.mixerParameter == 0)
        {
            mixer->setLevel(DrumMixerLevel(event.mixerValue));
        }
        else if (event.mixerParameter == 1)
        {
            mixer->setPanning(DrumMixerPanning(event.mixerValue));
        }
    }
}

void EventHandler::handleUnfinalizedNoteOn(
    const EventData &noteOnEvent, const std::optional<int> trackDevice,
    const std::optional<BusType> drumBusType)
{
    assert(noteOnEvent.type == sequencer::EventType::NoteOn &&
           noteOnEvent.duration == NoDuration);

    if (drumBusType.has_value() && isDrumNote(noteOnEvent.noteNumber))
    {
        const auto drumBus = mpc.getSequencer()->getDrumBus(*drumBusType);
        assert(drumBus);

        const auto note = noteOnEvent.noteNumber;

        const auto performanceDrum =
            mpc.getPerformanceManager().lock()->getSnapshot().getDrum(
                drumBus->getIndex());

        const auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
            0, performanceDrum, drumBus, mpc.getSampler(),
            mpc.getEngineHost()->getMixer(),
            mpc.screens->get<ScreenId::MixerSetupScreen>(),
            &mpc.getEngineHost()->getVoices(),
            mpc.getEngineHost()->getMixerConnections(), note,
            noteOnEvent.velocity, noteOnEvent.noteVariationType,
            noteOnEvent.noteVariationValue, 0, true, -1, -1);

        DrumNoteEventHandler::noteOn(ctx);
    }

    if (trackDevice.has_value())
    {
        constexpr int sampleNumber = 0;
        handleNoteEventMidiOut(noteOnEvent, *trackDevice, std::nullopt,
                               std::nullopt, sampleNumber);
    }
}

void EventHandler::handleNoteOffFromUnfinalizedNoteOn(
    const NoteNumber noteNumber, const std::optional<int> trackDevice,
    const std::optional<DrumBusIndex> drumBusIndex)
{
    if (drumBusIndex.has_value() && isDrumNote(noteNumber))
    {
        const auto drumBus = mpc.getSequencer()->getDrumBus(*drumBusIndex);

        assert(drumBus);

        const auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOffContext(
            0, drumBus, &mpc.getEngineHost()->getVoices(), noteNumber, -1);

        DrumNoteEventHandler::noteOff(ctx);
    }

    if (trackDevice.has_value())
    {
        constexpr int sampleNumber = 0;
        EventData noteOffEvent;
        noteOffEvent.type = EventType::NoteOff;
        noteOffEvent.noteNumber = noteNumber;
        handleNoteEventMidiOut(noteOffEvent, *trackDevice, std::nullopt,
                               std::nullopt, sampleNumber);
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
    const EventData &event, const int trackDevice,
    const std::optional<int> trackVelocityRatio,
    const std::optional<int> transposeAmount, const int sampleNumber)
{
    assert(event.type == EventType::NoteOn || event.type == EventType::NoteOff);

    if (const auto isRealTime =
            mpc.getEngineHost()->getAudioServer()->isRealTime();
        !isRealTime || trackDevice == 0)
    {
        return;
    }

    // Derives a 0-based MIDI channel.
    const int midiChannel = (trackDevice - 1) % 16;

    ClientMidiEvent msg;
    SeqEventToMidiEventMapper::mapSeqEventToMidiEvent(event, msg);
    msg.setChannel(midiChannel);
    msg.setBufferOffset(sampleNumber);

    if (transposeAmount)
    {
        msg.setNoteNumber(event.noteNumber + *transposeAmount);
    }

    if (event.type == EventType::NoteOn && trackVelocityRatio)
    {
        const auto velocityWithTrackVelocityRatioApplied =
            static_cast<int>(event.velocity * (*trackVelocityRatio * 0.01f));

        const auto velocityToUse =
            std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        msg.setVelocity(velocityToUse);
    }

    mpc.getMidiOutput()->enqueueEvent(msg);

    // For the MIDI output monitor screen
    notifyObservers(midiChannel < 16 ? "a" : "b" + std::to_string(midiChannel));
}
