#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Clock.hpp"
#include "sequencer/Track.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "file/wav/WavFile.hpp"

#include <thread>

TEST_CASE("Direct to disk recording does not start with silence",
          "[direct-to-disk-recording]")
{
    const int BUFFER_SIZE = 512;
    const int SAMPLE_RATE = 44100;
    const int DSP_CYCLE_DURATION_MICROSECONDS = 11601;
    const int DSP_CYCLE_COUNT = 2000000 / DSP_CYCLE_DURATION_MICROSECONDS;

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

    mpc.getSampler()->getProgram(0)->getNoteParameters(35)->setSoundIndex(0);

    auto seq = mpc.getSequencer()->getActiveSequence();
    seq->init(1);
    seq->setInitialTempo(300);
    auto event = seq->getTrack(0)->recordNoteEventSynced(0, 35, 127);
    seq->getTrack(0)->finalizeNoteEventSynced(event, 1);

    mpc.getLayeredScreen()->openScreen<VmpcDirectToDiskRecorderScreen>();

    auto audioMidiServices = mpc.getAudioMidiServices();
    auto audioServer = audioMidiServices->getAudioServer();

    audioServer->setSampleRate(SAMPLE_RATE);
    audioServer->resizeBuffers(BUFFER_SIZE);

    const float **inputBuffer = new const float *[2];
    inputBuffer[0] = new float[BUFFER_SIZE];
    inputBuffer[1] = new float[BUFFER_SIZE];

    float **outputBuffer = new float *[10];

    for (int i = 0; i < 10; ++i)
    {
        outputBuffer[i] = new float[BUFFER_SIZE];
    }

    int64_t timeInSamples = 0;

    auto audioThread = std::thread(
        [&]
        {
            for (int i = 0; i < DSP_CYCLE_COUNT; i++)
            {
                audioMidiServices->changeBounceStateIfRequired();
                mpc.getClock()->processBufferInternal(
                    mpc.getSequencer()->getTempo(), SAMPLE_RATE, BUFFER_SIZE,
                    0);
                audioServer->work(inputBuffer, outputBuffer, BUFFER_SIZE, {},
                                  {0, 1}, {}, {0, 1});
                timeInSamples += BUFFER_SIZE;
                std::this_thread::sleep_for(
                    std::chrono::microseconds(DSP_CYCLE_DURATION_MICROSECONDS));
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mpc.getScreen()->function(4);

    auto recordingsPath = mpc.paths->recordingsPath();

    for (const auto &entry : fs::directory_iterator(recordingsPath))
    {
        if (fs::is_directory(entry))
        {
            recordingsPath = entry.path();
            break;
        }
    }

    const auto recordingPath = recordingsPath / "L.wav";

    audioThread.join();

    REQUIRE(fs::exists(recordingPath));

    auto wavInputStream =
        std::make_shared<std::ifstream>(recordingPath, std::ios::binary);

    auto wavOrError = mpc::file::wav::WavFile::readWavStream(wavInputStream);

    assert(wavOrError.has_value());

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
