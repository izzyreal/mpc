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

#include "sequencer/NoteEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"

#include "Util.hpp"

#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "lcdgui/screens/TransScreen.hpp"

#include <cassert>
#include <memory>

using namespace mpc::audiomidi;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::engine;
using namespace mpc::engine::midi;

EventHandler::EventHandler(mpc::Mpc &mpcToUse) : mpc(mpcToUse)
{
    transposeCache.reserve(512);
}

void EventHandler::handleFinalizedEvent(const std::shared_ptr<Event> event,
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

        const auto durationTicks = *noteOnEvent->getDuration();

        const auto audioMidiServices = mpc.getAudioMidiServices();

        const auto audioServer = audioMidiServices->getAudioServer();

        const auto durationFrames = SeqUtil::ticksToFrames(
            durationTicks, mpc.getSequencer()->getTempo(),
            audioServer->getSampleRate());

        const auto frameSeq = audioMidiServices->getFrameSequencer();
        const auto eventFrameOffsetInBuffer = frameSeq->getEventFrameOffset();

        const uint64_t noteEventIdToUse = noteEventId++;
        auto drumBus = mpc.getSequencer()->getBus<DrumBus>(track->getBus());

        assert(drumBus);

        auto sampler = mpc.getSampler();
        const auto program = sampler->getProgram(drumBus->getProgram());
        const auto note = noteOnEvent->getNote();
        const int programPadIndex = program->getPadIndexFromNote(note);

        if (isDrumNote(noteOnEvent->getNote()) &&
            programPadIndex >= 0)
        {
            const auto velocityWithTrackVelocityRatioApplied =
                static_cast<int>(noteOnEvent->getVelocity() *
                                 (track->getVelocityRatio() * 0.01f));

            const auto velocityToUse =
                std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);
            const auto noteParameters =
                program->getNoteParameters(noteOnEvent->getNote());

            const auto sound =
                sampler->getSound(noteParameters->getSoundIndex());

            const auto voiceOverlapMode =
                (sound && sound->isLoopEnabled())
                    ? VoiceOverlapMode::NOTE_OFF
                    : noteParameters->getVoiceOverlapMode();

            int variationTypeToUse = noteOnEvent->getVariationType();
            int variationValueToUse = noteOnEvent->getVariationValue();

            const bool isSliderNote =
                program && program->getSlider()->getNote() == note;

            if (mpc.clientEventController->isAfterEnabled() && isSliderNote)
            {
                auto programSlider = program->getSlider();

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

                const auto sliderValue = mpc.getHardware()->getSlider()->getValue();

                auto [type, value] = Util::getSliderNoteVariationTypeAndValue(sliderNoteVariationContext);
                variationTypeToUse = type;
                variationValueToUse = value;

                if (mpc.getSequencer()->isOverdubbing())
                {
                    noteOnEvent->setVariationType(type);
                    noteOnEvent->setVariationValue(variationValueToUse);
                }
            }

            auto ctx = engine::DrumNoteEventContextBuilder::buildNoteOn(
                noteEventIdToUse, drumBus, mpc.getSampler(),
                mpc.getAudioMidiServices()->getMixer(),
                mpc.screens->get<MixerSetupScreen>(),
                &mpc.getAudioMidiServices()->getVoices(),
                mpc.getAudioMidiServices()->getMixerConnections(), note,
                velocityToUse,
                variationTypeToUse,
                variationValueToUse,
                eventFrameOffsetInBuffer,
                true, noteOnEvent->getTick(),
                voiceOverlapMode == VoiceOverlapMode::NOTE_OFF ? durationFrames
                                                               : -1);

            DrumNoteEventHandler::noteOn(ctx);

            program->registerNonMidiNoteOn(note, velocityToUse, programPadIndex,
                                      Program::NoteEventSource::SEQUENCED);

            const auto noteOffCtx = DrumNoteEventContextBuilder::buildNoteOff(
                noteEventIdToUse, drumBus,
                &mpc.getAudioMidiServices()->getVoices(),
                noteOnEvent->getNote(), noteOnEvent->getTick());

            auto drumNoteOffEvent = [program, programPadIndex, noteOffCtx]
            {
                program->registerNonMidiNoteOff(noteOffCtx.note,
                    programPadIndex, Program::NoteEventSource::SEQUENCED);
                DrumNoteEventHandler::noteOff(noteOffCtx);
            };

            frameSeq->enqueueEventAfterNFrames(
                drumNoteOffEvent, durationFrames + eventFrameOffsetInBuffer);
        }

        handleNoteEventMidiOut(event, track->getIndex(),
                               track->getDeviceIndex(),
                               track->getVelocityRatio());

        auto midiNoteOffEvent = [this, noteOffEvent = noteOnEvent->getNoteOff(),
                                 trackIndex = track->getIndex(),
                                 trackDeviceIndex = track->getDeviceIndex()]
        {
            handleNoteEventMidiOut(noteOffEvent, trackIndex, trackDeviceIndex,
                                   std::nullopt);
        };

        frameSeq->enqueueEventAfterNFrames(
            midiNoteOffEvent, durationFrames + eventFrameOffsetInBuffer);
    }
    else if (auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
             mixerEvent != nullptr)
    {
        auto pad = mixerEvent->getPad();
        auto sampler = mpc.getSampler();
        auto drumBus = mpc.getSequencer()->getBus<DrumBus>(track->getBus());

        assert(drumBus);

        auto program = sampler->getProgram(drumBus->getProgram());

        auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>();

        auto mixer = mixerSetupScreen->isStereoMixSourceDrum() ? drumBus->getStereoMixerChannels()[pad] : program->getStereoMixerChannel(pad);

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
    const std::shared_ptr<NoteOnEvent> noteOnEvent,
    const std::optional<int> trackIndex, const std::optional<int> trackDevice,
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
        const auto program = mpc.getSampler()->getProgram(drumBus->getProgram());
        const auto note = noteOnEvent->getNote();

        const auto velocityWithTrackVelocityRatioApplied =
            static_cast<int>(noteOnEvent->getVelocity() *
                             (trackVelocityRatio.value_or(100) * 0.01f));

        const auto velocityToUse =
            std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        auto ctx = engine::DrumNoteEventContextBuilder::buildNoteOn(
            0, drumBus, mpc.getSampler(), mpc.getAudioMidiServices()->getMixer(),
            mpc.screens->get<MixerSetupScreen>(),
            &mpc.getAudioMidiServices()->getVoices(),
            mpc.getAudioMidiServices()->getMixerConnections(), note,
            velocityToUse, noteOnEvent->getVariationType(),
            noteOnEvent->getVariationValue(), 0, true, -1, -1);

        DrumNoteEventHandler::noteOn(ctx);
    }

    if (trackIndex.has_value() && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOnEvent, *trackIndex, *trackDevice,
                               trackVelocityRatio);
    }
}

// Input from physical pad releases
void EventHandler::handleNoteOffFromUnfinalizedNoteOn(
    const std::shared_ptr<NoteOffEvent> noteOffEvent,
    const std::optional<int> trackIndex, const std::optional<int> trackDevice,
    const std::optional<int> drumIndex)
{
    assert(noteOffEvent);

    if (drumIndex.has_value() && isDrumNote(noteOffEvent->getNote()))
    {
        auto drumBus = mpc.getSequencer()->getDrumBus(*drumIndex);

        assert(drumBus);

        const auto program = mpc.getSampler()->getProgram(drumBus->getProgram());
        const auto note = noteOffEvent->getNote();

        auto ctx = DrumNoteEventContextBuilder::buildNoteOff(
            0, drumBus, &mpc.getAudioMidiServices()->getVoices(),
            noteOffEvent->getNote(), -1);

        DrumNoteEventHandler::noteOff(ctx);
    }

    if (trackIndex.has_value() && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOffEvent, *trackIndex, *trackDevice,
                               std::nullopt);
    }
}

void EventHandler::handleMidiInputNoteOn(const int midiChannelIndex,
    const std::shared_ptr<NoteOnEventPlayOnly> noteOnEvent,
    const int frameOffsetInBuffer, const int trackIndex, const int trackDevice,
    const int trackVelocityRatio, const std::optional<int> drumIndex)
{
    assert(noteOnEvent);
    assert(!noteOnEvent->isFinalized());

    handleNoteEventMidiOut(noteOnEvent, trackIndex, trackDevice,
                           trackVelocityRatio);

    if (!drumIndex.has_value())
    {
        // No further processing applicable.
        return;
    }

    assert(*drumIndex >= 0 && *drumIndex <= 3);

    // This is a DRUM track, so we trigger sample playback and register a
    // program pad press.
    auto drumBus = mpc.getSequencer()->getDrumBus(*drumIndex);

    assert(drumBus);

    const auto program = mpc.getSampler()->getProgram(drumBus->getProgram());
    const auto note = noteOnEvent->getNote();

    const auto velocityWithTrackVelocityRatioApplied = static_cast<int>(
        noteOnEvent->getVelocity() * (trackVelocityRatio * 0.01f));

    const auto velocityToUse =
        std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

    program->registerMidiNoteOn(midiChannelIndex, note, velocityToUse, program->getPadIndexFromNote(note), Program::NoteEventSource::MIDI);

    const bool isSoundScreen = lcdgui::screengroups::isSoundScreen(mpc.getLayeredScreen()->getCurrentScreen());

    if (isSoundScreen)
    {
        mpc.getBasicPlayer().mpcNoteOn(mpc.getSampler()->getSoundIndex(), 127, 0);
        return;
    }

    const bool isSequencerScreen = mpc.getLayeredScreen()->isCurrentScreen<SequencerScreen>();

    const bool isNoteRepeatLockedOrPressed =
        mpc.clientEventController->clientHardwareEventController
            ->isNoteRepeatLocked() ||
        mpc.getHardware()
            ->getButton(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)
            ->isPressed();

    if (isSequencerScreen && isNoteRepeatLockedOrPressed &&
        mpc.getSequencer()->isPlaying())
    {
        return;
    }

    auto ctx = engine::DrumNoteEventContextBuilder::buildNoteOn(
        0, drumBus, mpc.getSampler(), mpc.getAudioMidiServices()->getMixer(),
        mpc.screens->get<MixerSetupScreen>(),
        &mpc.getAudioMidiServices()->getVoices(),
        mpc.getAudioMidiServices()->getMixerConnections(), note, velocityToUse,
        noteOnEvent->getVariationType(), noteOnEvent->getVariationValue(),
        frameOffsetInBuffer, true, -1, -1);

    DrumNoteEventHandler::noteOn(ctx);
}

void EventHandler::handleMidiInputNoteOff(
    const int midiChannelIndex,
    const std::shared_ptr<NoteOffEvent> noteOffEvent,
    const int frameOffsetInBuffer, const int trackIndex, const int trackDevice,
    const std::optional<int> drumIndex)
{
    assert(noteOffEvent);

    const auto noteOffMidiOutEvent =
        [this, noteOffEvent, trackIndex, trackDevice]
    {
        handleNoteEventMidiOut(noteOffEvent, trackIndex, trackDevice,
                               std::nullopt);
    };

    const auto frameSeq = mpc.getAudioMidiServices()->getFrameSequencer();

    frameSeq->enqueueEventAfterNFrames(noteOffMidiOutEvent,
                                       frameOffsetInBuffer);

    if (!drumIndex.has_value())
    {
        // No further processing applicable.
        return;
    }

    assert(*drumIndex >= 0 && *drumIndex <= 3);

    // This is a DRUM track, so we stop sample playback and register a program
    // pad release.

    auto drumBus = mpc.getSequencer()->getDrumBus(*drumIndex);

    assert(drumBus);

    const auto drumNoteOffEvent =
        [drumBus, note = noteOffEvent->getNote(),
         voices = &mpc.getAudioMidiServices()->getVoices(),
         program = mpc.getSampler()->getProgram(drumBus->getProgram()), midiChannelIndex]
    {
        auto ctx = DrumNoteEventContextBuilder::buildNoteOff(0, drumBus, voices,
                                                             note, -1);

        DrumNoteEventHandler::noteOff(ctx);
        program->registerMidiNoteOff(midiChannelIndex, note, program->getPadIndexFromNote(note),
                                    Program::NoteEventSource::MIDI);
    };

    frameSeq->enqueueEventAfterNFrames(drumNoteOffEvent, frameOffsetInBuffer);
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
    const std::shared_ptr<Event> event, const int trackIndex,
    const int trackDevice, const std::optional<int> trackVelocityRatio)
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

    std::shared_ptr<ShortMessage> msg;

    int transposeAmount = 0;

    if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event))
    {
        auto transScreen = mpc.screens->get<TransScreen>();

        if (transScreen->transposeAmount != 0 && trackIndex < 64 &&
            (transScreen->tr == -1 || transScreen->tr == trackIndex))
        {
            transposeAmount = transScreen->transposeAmount;
            transposeCache[noteOnEvent->getNoteOff()] = transposeAmount;
        }

        msg = noteOnEvent->createShortMessage(midiChannel, transposeAmount);

        const auto velocityWithTrackVelocityRatioApplied = static_cast<int>(
            noteOnEvent->getVelocity() * (*trackVelocityRatio * 0.01f));

        const auto velocityToUse =
            std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        msg->setMessage(msg->getStatus(), msg->getChannel(), msg->getData1(),
                        velocityToUse);

        const auto audioMidiServices = mpc.getAudioMidiServices();
        const auto frameSeq = audioMidiServices->getFrameSequencer();
        const auto audioServer = audioMidiServices->getAudioServer();

        if (noteOnEvent->isFinalized())
        {
            frameSeq->enqueueEventAfterNFrames(
                [this, noteOnEvent, trackIndex, trackDevice]
                {
                    handleNoteEventMidiOut(noteOnEvent->getNoteOff(),
                                           trackIndex, trackDevice,
                                           std::nullopt);
                },
                SeqUtil::ticksToFrames(*noteOnEvent->getDuration(),
                                       mpc.getSequencer()->getTempo(),
                                       audioServer->getSampleRate()));
        }
    }
    else if (auto noteOffEvent = std::dynamic_pointer_cast<NoteOffEvent>(event))
    {
        auto candidate = transposeCache.find(noteOffEvent);

        if (candidate != end(transposeCache) && trackIndex < 64)
        {
            transposeAmount = candidate->second;
            transposeCache.erase(candidate);
        }

        msg = noteOffEvent->createShortMessage(midiChannel, transposeAmount);
    }

    const auto directToDiskRecorderScreen =
        mpc.screens->get<VmpcDirectToDiskRecorderScreen>();

    if (!(mpc.getAudioMidiServices()->isBouncing() &&
          directToDiskRecorderScreen->offline) &&
        trackDevice != 0)
    {
        msg->bufferPos = mpc.getAudioMidiServices()
                             ->getFrameSequencer()
                             ->getEventFrameOffset();
        mpc.getMidiOutput()->enqueueMessageOutputA(msg);
    }

    // For the MIDI output monitor screen
    notifyObservers(midiChannel < 16 ? "a" : "b" + std::to_string(midiChannel));
}
