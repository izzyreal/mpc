#include "EventHandler.hpp"

#include "Mpc.hpp"

#include "engine/DrumNoteEventContextBuilder.hpp"

#include "audiomidi/AudioMidiServices.hpp"

#include "audiomidi/MidiOutput.hpp"
#include "engine/Drum.hpp"
#include "engine/DrumNoteEventHandler.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"

#include "sequencer/NoteEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"

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

void EventHandler::handleFinalizedEvent(const std::shared_ptr<Event> event, Track *const track)
{
    if (mpc.getSequencer()->isCountingIn())
    {
        return;
    }

    if (std::dynamic_pointer_cast<TempoChangeEvent>(event))
    {
        return;
    }

    if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event); noteOnEvent)
    {
        assert(noteOnEvent->getDuration().has_value() && *noteOnEvent->getDuration() >= 0);

        const auto durationTicks = *noteOnEvent->getDuration();

        const auto audioMidiServices = mpc.getAudioMidiServices();

        const auto audioServer = audioMidiServices->getAudioServer();

        const auto durationFrames = SeqUtil::ticksToFrames(durationTicks, mpc.getSequencer()->getTempo(),
                audioServer->getSampleRate());

        const auto frameSeq = audioMidiServices->getFrameSequencer();

        if (const int drumIndex = track->getBus() - 1; drumIndex >= 0)
        {
            auto &drum = mpc.getDrum(drumIndex);
            auto sampler = mpc.getSampler();
            const auto program = sampler->getProgram(drum.getProgram());
            const auto note = noteOnEvent->getNote();

            const auto velocityWithTrackVelocityRatioApplied =
                static_cast<int>(noteOnEvent->getVelocity() * (track->getVelocityRatio() * 0.01f));

            const auto velocityToUse = std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);
            const auto noteParameters = program->getNoteParameters(noteOnEvent->getNote());

            const auto sound = sampler->getSound(noteParameters->getSoundIndex());

            const auto voiceOverlap = (sound && sound->isLoopEnabled()) ? 2 : noteParameters->getVoiceOverlap();

            auto ctx = engine::DrumNoteEventContextBuilder::buildNoteOn(
                    &drum,
                    mpc.getSampler(),
                    mpc.getAudioMidiServices()->getMixer(),
                    mpc.screens->get<MixerSetupScreen>(),
                    mpc.getAudioMidiServices()->getVoices(),
                    mpc.getAudioMidiServices()->getMixerConnections(),
                    note,
                    velocityToUse,
                    noteOnEvent->getVariationType(),
                    noteOnEvent->getVariationValue(),
                    frameSeq->getEventFrameOffset(),
                    true,
                    noteOnEvent->getTick(),
                    voiceOverlap == 2 ? durationFrames : -1
            );

            DrumNoteEventHandler::noteOn(ctx);

            const int programPadIndex = program->getPadIndexFromNote(note); 

            program->registerPadPress(programPadIndex, Program::PadPressSource::NON_PHYSICAL);

            const auto noteOffCtx = DrumNoteEventContextBuilder::buildNoteOff(
                &drum,
                mpc.getAudioMidiServices()->getVoices(),
                noteOnEvent->getNote(),
                0,
                noteOnEvent->getTick()
            );

            auto drumNoteOffEvent = [
                program,
                programPadIndex,
                noteOffCtx
            ] (int /*frameOffsetInBuffer*/)
            {
                program->registerPadRelease(programPadIndex, Program::PadPressSource::NON_PHYSICAL);
                DrumNoteEventHandler::noteOff(noteOffCtx);
            };

            frameSeq->enqueueEventAfterNFrames(drumNoteOffEvent, durationFrames);
        }

        handleNoteEventMidiOut(event, track->getIndex(), track->getDeviceIndex(), track->getVelocityRatio());

        auto midiNoteOffEvent = [
            this,
            noteOffEvent = noteOnEvent->getNoteOff(),
            trackIndex = track->getIndex(),
            trackDeviceIndex = track->getDeviceIndex()
        ] (int /*frameOffsetInBuffer*/)
        {
            handleNoteEventMidiOut(noteOffEvent, trackIndex, trackDeviceIndex, std::nullopt);
        };

        frameSeq->enqueueEventAfterNFrames(midiNoteOffEvent, durationFrames);
    }
    else if (auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event); mixerEvent != nullptr)
    {
        auto pad = mixerEvent->getPad();
        auto sampler = mpc.getSampler();
        auto p = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()));
        auto mixer = p->getStereoMixerChannel(pad);

        auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>();

        if (mixerSetupScreen->isStereoMixSourceDrum())
        {
            if (const int drumIndex = track->getBus() - 1; drumIndex >= 0)
            {
                auto &drum = mpc.getDrum(drumIndex);
                mixer = drum.getStereoMixerChannels()[pad];
            }
            else
            {
                return;
            }
        }

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

void EventHandler::handleUnfinalizedNoteOn(const std::shared_ptr<NoteOnEvent> noteOnEvent,
        const std::optional<int> trackIndex,
        const std::optional<int> trackDevice,
        const std::optional<int> trackVelocityRatio,
        const std::optional<int> drumIndex)
{
    assert(noteOnEvent);
    assert(!noteOnEvent->isFinalized());

    if (drumIndex.has_value())
    {
        auto &drum = mpc.getDrum(*drumIndex);
        const auto program = mpc.getSampler()->getProgram(drum.getProgram());
        const auto note = noteOnEvent->getNote();

        const auto velocityWithTrackVelocityRatioApplied =
            static_cast<int>(noteOnEvent->getVelocity() * (trackVelocityRatio.value_or(100) * 0.01f));

        const auto velocityToUse = std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        auto ctx = engine::DrumNoteEventContextBuilder::buildNoteOn(
            &drum,
            mpc.getSampler(),
            mpc.getAudioMidiServices()->getMixer(),
            mpc.screens->get<MixerSetupScreen>(),
            mpc.getAudioMidiServices()->getVoices(),
            mpc.getAudioMidiServices()->getMixerConnections(),
            note,
            velocityToUse,
            noteOnEvent->getVariationType(),
            noteOnEvent->getVariationValue(),
            0,
            true,
            -1,
            -1
        );

        DrumNoteEventHandler::noteOn(ctx);

        program->registerPadPress(program->getPadIndexFromNote(note), Program::PadPressSource::PHYSICAL);
    }

    if (trackIndex.has_value() && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOnEvent, *trackIndex, *trackDevice, trackVelocityRatio);
    }
}

// Input from physical pad releases
void EventHandler::handleNoteOffFromUnfinalizedNoteOn(const std::shared_ptr<NoteOffEvent> noteOffEvent,
        const std::optional<int> trackIndex,
        const std::optional<int> trackDevice,
        const std::optional<int> drumIndex)
{
    assert(noteOffEvent);

    if (drumIndex.has_value())
    {
        auto &drum = mpc.getDrum(*drumIndex);
        const auto program = mpc.getSampler()->getProgram(drum.getProgram());
        const auto note = noteOffEvent->getNote();

        auto ctx = DrumNoteEventContextBuilder::buildNoteOff(
            &drum,
            mpc.getAudioMidiServices()->getVoices(),
            noteOffEvent->getNote(),
            0,
            -1
        );

        DrumNoteEventHandler::noteOff(ctx);

        program->registerPadRelease(program->getPadIndexFromNote(note), Program::PadPressSource::PHYSICAL);
    }

    if (trackIndex.has_value() && trackDevice.has_value())
    {
        handleNoteEventMidiOut(noteOffEvent, *trackIndex, *trackDevice, std::nullopt);
    }
}

void EventHandler::handleMidiInputNoteOn(const std::shared_ptr<NoteOnEventPlayOnly> noteOnEvent,
        const int frameOffsetInBuffer,
        const int trackIndex,
        const int trackDevice,
        const int trackVelocityRatio,
        const std::optional<int> drumIndex)
{
    assert(noteOnEvent);
    assert(!noteOnEvent->isFinalized());

    handleNoteEventMidiOut(noteOnEvent, trackIndex, trackDevice, trackVelocityRatio);

    if (!drumIndex.has_value())
    {
        // No further processing applicable.
        return;
    }

    assert(*drumIndex >= 0 && *drumIndex <= 3);

    // This is a DRUM track, so we trigger sample playback and register a program pad press.
    auto &drum = mpc.getDrum(*drumIndex);
    const auto program = mpc.getSampler()->getProgram(drum.getProgram());
    const auto note = noteOnEvent->getNote();

    const auto velocityWithTrackVelocityRatioApplied =
        static_cast<int>(noteOnEvent->getVelocity() * (trackVelocityRatio * 0.01f));

    const auto velocityToUse = std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

    auto ctx = engine::DrumNoteEventContextBuilder::buildNoteOn(
            &drum,
            mpc.getSampler(),
            mpc.getAudioMidiServices()->getMixer(),
            mpc.screens->get<MixerSetupScreen>(),
            mpc.getAudioMidiServices()->getVoices(),
            mpc.getAudioMidiServices()->getMixerConnections(),
            note,
            velocityToUse,
            noteOnEvent->getVariationType(),
            noteOnEvent->getVariationValue(),
            frameOffsetInBuffer,
            true,
            -1,
            -1
    );

    DrumNoteEventHandler::noteOn(ctx);

    program->registerPadPress(program->getPadIndexFromNote(note), Program::PadPressSource::NON_PHYSICAL);
}

void EventHandler::handleMidiInputNoteOff(const std::shared_ptr<NoteOffEvent> noteOffEvent,
        const int frameOffsetInBuffer,
        const int trackIndex,
        const int trackDevice,
        const std::optional<int> drumIndex)
{
    assert(noteOffEvent);

    handleNoteEventMidiOut(noteOffEvent, trackIndex, trackDevice, std::nullopt);

    if (!drumIndex.has_value())
    {
        // No further processing applicable.
        return;
    }

    assert(*drumIndex >= 0 && *drumIndex <= 3);

    // This is a DRUM track, so we stop sample playback and register a program pad release.
    auto &drum = mpc.getDrum(*drumIndex);
    const auto program = mpc.getSampler()->getProgram(drum.getProgram());
    const auto note = noteOffEvent->getNote();

    auto ctx = DrumNoteEventContextBuilder::buildNoteOff(
        &drum,
        mpc.getAudioMidiServices()->getVoices(),
        noteOffEvent->getNote(),
        frameOffsetInBuffer,
        -1
    );

    DrumNoteEventHandler::noteOff(ctx);

    program->registerPadRelease(program->getPadIndexFromNote(note), Program::PadPressSource::NON_PHYSICAL);
}

/**
 * Creates MIDI note-on and note-off events and queues them for emission to the MIDI output port.
 *
 * The event must be a NoteOnEvent or NoteOffEvent produced by the sequencer or by physical pad
 * presses. We don't deal with MIDI thru here. That is done by mpc::audiomidi::MidiInput.
 *
 * If the NoteOnEvent is finalized (i.e. already has a duration, such as a note event in a sequence),
 * its MIDI note-off counterpart is immediately scheduled for emission after the appropriate number of
 * audio frames has passsed.
 *
 * If the NoteOnEvent is not finalized (i.e. has no duration yet, such as those generated by realtime
 * pad presses), the caller is responsible for invoking handleNoteEventMidiOut at the appropriate time.
 */
void EventHandler::handleNoteEventMidiOut(const std::shared_ptr<Event> event, const int trackIndex, const int trackDevice, const std::optional<int> trackVelocityRatio)
{
    assert(std::dynamic_pointer_cast<NoteOnEvent>(event) ||
            std::dynamic_pointer_cast<NoteOffEvent>(event));

    assert(trackVelocityRatio.has_value() || std::dynamic_pointer_cast<NoteOffEvent>(event));

    const auto isRealTime = mpc.getAudioMidiServices()->getAudioServer()->isRealTime();

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

        if (transScreen->transposeAmount != 0 && trackIndex < 64 && (transScreen->tr == -1 || transScreen->tr == trackIndex))
        {
            transposeAmount = transScreen->transposeAmount;
            transposeCache[noteOnEvent->getNoteOff()] = transposeAmount;
        }

        msg = noteOnEvent->createShortMessage(midiChannel, transposeAmount);

        const auto velocityWithTrackVelocityRatioApplied =
            static_cast<int>(noteOnEvent->getVelocity() * (*trackVelocityRatio * 0.01f));

        const auto velocityToUse = std::clamp(velocityWithTrackVelocityRatioApplied, 1, 127);

        msg->setMessage(msg->getStatus(), msg->getChannel(), msg->getData1(), velocityToUse);

        const auto audioMidiServices = mpc.getAudioMidiServices();
        const auto frameSeq = audioMidiServices->getFrameSequencer();
        const auto audioServer = audioMidiServices->getAudioServer();

        if (noteOnEvent->isFinalized())
        {
            frameSeq->enqueueEventAfterNFrames([this, noteOnEvent, trackIndex, trackDevice](int)
                    {
                    handleNoteEventMidiOut(noteOnEvent->getNoteOff(), trackIndex, trackDevice, std::nullopt);
                    },

                    SeqUtil::ticksToFrames(*noteOnEvent->getDuration(), mpc.getSequencer()->getTempo(), audioServer->getSampleRate())
                    );
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

    const auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>();

    if (!(mpc.getAudioMidiServices()->isBouncing() && directToDiskRecorderScreen->offline) && trackDevice != 0)
    {
        msg->bufferPos = mpc.getAudioMidiServices()->getFrameSequencer()->getEventFrameOffset();
        mpc.getMidiOutput()->enqueueMessageOutputA(msg);
    }

    // For the MIDI output monitor screen
    notifyObservers(midiChannel < 16 ? "a" : "b" + std::to_string(midiChannel));
}

