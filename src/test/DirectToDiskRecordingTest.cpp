#include <catch2/catch_test_macros.hpp>
#include "IntTypes.hpp"
#include "sequencer/Transport.hpp"

#include "TestMpc.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Clock.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "engine/EngineHost.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "file/wav/WavFile.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui;
using namespace mpc::sequencer;

TEST_CASE("Direct to disk recording does not start with silence",
          "[direct-to-disk-recording]")
{
    constexpr int BUFFER_SIZE = 512;
    constexpr int SAMPLE_RATE = 44100;
    constexpr int DSP_CYCLE_DURATION_MICROSECONDS = 11601;
    constexpr int DSP_CYCLE_COUNT = 2000000 / DSP_CYCLE_DURATION_MICROSECONDS;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sound = mpc.getSampler()->addSound();
    assert(sound != nullptr);
    sound->setMono(true);

    for (int i = 0; i < 1000; i++)
    {
        sound->insertFrame({1.f}, i);
    }

    sound->setStart(0);
    sound->setEnd(1000);

    mpc.getSampler()
        ->getProgram(0)
        ->getNoteParameters(mpc::MinDrumNoteNumber)
        ->setSoundIndex(0);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(1);
    stateManager->drainQueue();
    seq->setInitialTempo(300);
    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = mpc::MinDrumNoteNumber;
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(1);
    seq->getTrack(0)->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    auto engineHost = mpc.getEngineHost();
    auto audioServer = engineHost->getAudioServer();

    audioServer->setSampleRate(SAMPLE_RATE);
    audioServer->resizeBuffers(BUFFER_SIZE);
    audioServer->start();

    const float **inputBuffer = new const float *[2];
    inputBuffer[0] = new float[BUFFER_SIZE];
    inputBuffer[1] = new float[BUFFER_SIZE];

    float **outputBuffer = new float *[10];

    for (int i = 0; i < 10; ++i)
    {
        outputBuffer[i] = new float[BUFFER_SIZE];
    }

    auto ls = mpc.getLayeredScreen();
    ls->openScreenById(ScreenId::VmpcDirectToDiskRecorderScreen);
    ls->timerCallback();

    mpc.getScreen()->function(4);

    for (int i = 0; i < DSP_CYCLE_COUNT; i++)
    {
        engineHost->changeBounceStateIfRequired();
        engineHost->prepareProcessBlock(BUFFER_SIZE);
        mpc.getClock()->processBufferInternal(
            sequencer->getTransport()->getTempo(), SAMPLE_RATE, BUFFER_SIZE, 0);
        audioServer->work(inputBuffer, outputBuffer, BUFFER_SIZE, {}, {0, 1},
                          {}, {0, 1});
    }

    auto recordingsPath = mpc.paths->getDocuments()->recordingsPath();

    for (const auto &entry : fs::directory_iterator(recordingsPath))
    {
        if (fs::is_directory(entry))
        {
            recordingsPath = entry.path();
            break;
        }
    }

    const auto recordingPath = recordingsPath / "L.wav";

    REQUIRE(fs::exists(recordingPath));

    auto wavInputStream =
        std::make_shared<std::ifstream>(recordingPath, std::ios::binary);

    auto wavOrError = mpc::file::wav::WavFile::readWavStream(wavInputStream);

    REQUIRE(wavOrError.has_value());

    auto wav = wavOrError.value();
    std::vector<float> wavFrames(100);
    wav->readFrames(wavFrames, 100);

    for (int i = 0; i < 100; i++)
    {
        REQUIRE(wavFrames[i] > 0.f);
    }

    for (int i = 0; i < 10; ++i)
    {
        if (i < 2)
        {
            delete[] inputBuffer[i];
        }
        delete[] outputBuffer[i];
    }

    delete[] inputBuffer;
    delete[] outputBuffer;
}

TEST_CASE(
    "Direct to disk recording yields same recording when invoked multiple "
    "times",
    "[direct-to-disk-recording]")
{
    constexpr int BUFFER_SIZE = 512;
    constexpr int SAMPLE_RATE = 44100;
    constexpr int DSP_CYCLE_DURATION_MICROSECONDS = 11601;
    constexpr int DSP_CYCLE_COUNT = 2000000 / DSP_CYCLE_DURATION_MICROSECONDS;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sound = mpc.getSampler()->addSound();
    assert(sound != nullptr);
    sound->setMono(true);

    for (int i = 0; i < 1000; i++)
    {
        sound->insertFrame({.5f}, i);
    }

    sound->setStart(0);
    sound->setEnd(1000);

    mpc.getSampler()
        ->getProgram(0)
        ->getNoteParameters(mpc::MinDrumNoteNumber)
        ->setSoundIndex(0);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(1);
    stateManager->drainQueue();
    seq->setInitialTempo(300);
    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = mpc::MinDrumNoteNumber;
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(1);
    seq->getTrack(0)->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    auto engineHost = mpc.getEngineHost();
    auto audioServer = engineHost->getAudioServer();

    audioServer->setSampleRate(SAMPLE_RATE);
    audioServer->resizeBuffers(BUFFER_SIZE);
    audioServer->start();

    const float **inputBuffer = new const float *[2];
    inputBuffer[0] = new float[BUFFER_SIZE];
    inputBuffer[1] = new float[BUFFER_SIZE];

    float **outputBuffer = new float *[10];
    for (int i = 0; i < 10; ++i)
    {
        outputBuffer[i] = new float[BUFFER_SIZE];
    }

    auto ls = mpc.getLayeredScreen();

    auto runRecording = [&]
    {
        ls->openScreenById(ScreenId::VmpcDirectToDiskRecorderScreen);

        ls->timerCallback();

        mpc.getScreen()->function(4);

        for (int i = 0; i < DSP_CYCLE_COUNT; i++)
        {
            engineHost->changeBounceStateIfRequired();
            engineHost->prepareProcessBlock(BUFFER_SIZE);
            mpc.getClock()->processBufferInternal(
                sequencer->getTransport()->getTempo(), SAMPLE_RATE, BUFFER_SIZE,
                0);
            audioServer->work(inputBuffer, outputBuffer, BUFFER_SIZE, {},
                              {0, 1}, {}, {0, 1});
        }

        auto recordingsPath = mpc.paths->getDocuments()->recordingsPath();

        for (const auto &entry : fs::directory_iterator(recordingsPath))
        {
            if (fs::is_directory(entry))
            {
                recordingsPath = entry.path();
                break;
            }
        }

        auto recordingPath = recordingsPath / "L.wav";
        REQUIRE(fs::exists(recordingPath));

        std::vector<float> frames(100);

        {
            auto wavInputStream =
                std::make_shared<std::ifstream>(recordingPath, std::ios::binary);
            auto wavOrError =
                mpc::file::wav::WavFile::readWavStream(wavInputStream);
            REQUIRE(wavOrError.has_value());

            auto wav = wavOrError.value();
            wav->readFrames(frames, 100);
        }

        fs::remove_all(recordingsPath);
        REQUIRE(!fs::exists(recordingsPath));
        return frames;
    };

    auto firstFrames = runRecording();

    auto secondFrames = runRecording();

    REQUIRE(firstFrames.size() == secondFrames.size());

    for (size_t i = 0; i < firstFrames.size(); ++i)
    {
        REQUIRE(firstFrames[i] == secondFrames[i]);
    }

    for (int i = 0; i < 10; ++i)
    {
        if (i < 2)
        {
            delete[] inputBuffer[i];
        }
        delete[] outputBuffer[i];
    }

    delete[] inputBuffer;
    delete[] outputBuffer;
}
