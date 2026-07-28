#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "engine/EngineHost.hpp"
#include "engine/Voice.hpp"
#include "engine/audio/core/AudioBuffer.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

#include <algorithm>
#include <memory>
#include <vector>

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::engine;
using namespace mpc::engine::audio::core;
using namespace mpc::sequencer;

namespace
{
    constexpr int kBufferSize = 64;
    constexpr int kNote = 35;

    void drain(Mpc &mpc)
    {
        for (int i = 0; i < 3; ++i)
        {
            mpc.getEngineHost()->drainAudioThreadStateQueues();
        }
    }

    void prepareDrumSound(Mpc &mpc)
    {
        const auto sequencer = mpc.getSequencer();
        sequencer->getSequence(0)->init(1);
        drain(mpc);

        const auto track = sequencer->getSequence(0)->getTrack(0);
        track->setBusType(BusType::DRUM1);
        sequencer->setSelectedTrackIndex(TrackIndex(0));

        const auto sampler = mpc.getSampler();
        const auto sound = sampler->addSound();
        REQUIRE(sound);
        sound->setSampleData(std::make_shared<std::vector<float>>(200000, 1.f));
        sound->setMono(true);
        sound->setLevel(100);

        const auto program = sampler->getProgram(0);
        REQUIRE(program);
        const auto noteParameters = program->getNoteParameters(kNote);
        noteParameters->setSoundIndex(0);
        noteParameters->setVoiceOverlapMode(
            sampler::VoiceOverlapMode::NOTE_OFF);

        sequencer->getDrumBus(DrumBusIndex(0))
            ->setProgramIndex(ProgramIndex(0));
        drain(mpc);
    }

    ClientMidiEvent makeNoteEvent(const ClientMidiEvent::MessageType type,
                                  const int frameOffset)
    {
        ClientMidiEvent result{type};
        result.setChannel(0);
        result.setNoteNumber(kNote);
        result.setVelocity(127);
        result.setBufferOffset(frameOffset);
        return result;
    }

    std::shared_ptr<Voice> getActiveVoice(Mpc &mpc)
    {
        const auto &voices = mpc.getEngineHost()->getVoices();
        const auto it = std::find_if(voices.begin(), voices.end(),
                                     [](const auto &voice)
                                     {
                                         return !voice->isFinished() &&
                                                voice->getNote() == kNote;
                                     });
        REQUIRE(it != voices.end());
        return *it;
    }
} // namespace

TEST_CASE("Incoming MIDI note-on preserves its audio-block frame offset",
          "[midi-input][timing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareDrumSound(mpc);

    constexpr int frameOffset = 17;
    auto event = makeNoteEvent(ClientMidiEvent::NOTE_ON, frameOffset);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    mpc.getEngineHost()->drainAudioThreadStateQueues();

    const auto voice = getActiveVoice(mpc);
    AudioBuffer buffer("MIDI note-on timing", 2, kBufferSize, 44100);
    voice->processAudio(&buffer, kBufferSize);

    const auto &left = buffer.getChannel(0);
    REQUIRE(std::all_of(left.begin(), left.begin() + frameOffset,
                        [](const float sample)
                        {
                            return sample == 0.f;
                        }));
    REQUIRE(std::any_of(left.begin() + frameOffset, left.end(),
                        [](const float sample)
                        {
                            return sample != 0.f;
                        }));
}

TEST_CASE("Incoming MIDI note-off preserves its audio-block frame offset",
          "[midi-input][timing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareDrumSound(mpc);

    auto noteOn = makeNoteEvent(ClientMidiEvent::NOTE_ON, 0);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(noteOn);
    mpc.getEngineHost()->drainAudioThreadStateQueues();

    const auto voice = getActiveVoice(mpc);
    REQUIRE_FALSE(voice->isDecaying());

    constexpr int frameOffset = 11;
    auto noteOff = makeNoteEvent(ClientMidiEvent::NOTE_OFF, frameOffset);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(noteOff);
    mpc.getEngineHost()->drainAudioThreadStateQueues();

    REQUIRE_FALSE(voice->isDecaying());

    AudioBuffer beforeOffset("Before MIDI note-off", 2, frameOffset - 1, 44100);
    voice->processAudio(&beforeOffset, frameOffset - 1);
    REQUIRE_FALSE(voice->isDecaying());

    AudioBuffer atOffset("At MIDI note-off", 2, 1, 44100);
    voice->processAudio(&atOffset, 1);
    REQUIRE(voice->isDecaying());
}
