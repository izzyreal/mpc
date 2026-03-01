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

#include <thread>
#include <chrono>

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

    auto recordingsRoot = mpc.paths->getDocuments()->recordingsPath();
    (void) mpc_fs::create_directories(recordingsRoot);
    if (const auto dirItRes = mpc_fs::make_directory_iterator(recordingsRoot))
    {
        for (auto e = *dirItRes; e != mpc_fs::directory_end(); ++e)
        {
            (void) mpc_fs::remove_all(e->path());
        }
    }

    auto sound = mpc.getSampler()->addSound();
    assert(sound != nullptr);
    sound->setMono(true);

    for (int i = 0; i < 1000; i++)
        sound->insertFrame({1.f}, i);

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

    const float** inputBuffer = new const float*[2];
    inputBuffer[0] = new float[BUFFER_SIZE]();
    inputBuffer[1] = new float[BUFFER_SIZE]();

    float** outputBuffer = new float*[10];
    for (int i = 0; i < 10; ++i)
        outputBuffer[i] = new float[BUFFER_SIZE]();

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

    mpc.getScreen()->function(4);
    ls->timerCallback();

    mpc_fs::path recordingPath;
    mpc_fs::file_time_type bestTime{};
    bool have = false;

    for (int tries = 0; tries < 400 && !have; ++tries)
    {
        const auto recDirItRes =
            mpc_fs::make_recursive_directory_iterator(recordingsRoot);
        if (!recDirItRes)
        {
            break;
        }
        for (auto entry = *recDirItRes; entry != mpc_fs::recursive_directory_end();
             ++entry)
        {
            if (!entry->is_regular_file())
                continue;

            if (entry->path().filename() != "L.wav")
                continue;

            const auto t = mpc_fs::last_write_time(entry->path());
            if (t && (!have || *t > bestTime))
            {
                have = true;
                bestTime = *t;
                recordingPath = entry->path();
            }
        }

        if (!have)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    REQUIRE(have);
    REQUIRE(mpc_fs::exists(recordingPath).value_or(false));

    constexpr std::uintmax_t WAV_HEADER_BYTES = 44;
    constexpr std::uintmax_t MIN_DATA_BYTES_FOR_100_MONO_SAMPLES = 100 * 2;

    std::uintmax_t lastSize = 0;
    int stable = 0;
    bool haveSize = false;

    for (int tries = 0; tries < 400 && stable < 5; ++tries)
    {
        const auto size = mpc_fs::file_size(recordingPath);
        if (size && *size >= WAV_HEADER_BYTES + MIN_DATA_BYTES_FOR_100_MONO_SAMPLES)
        {
            if (!haveSize)
            {
                haveSize = true;
                lastSize = *size;
                stable = 1;
            }
            else if (*size == lastSize)
            {
                ++stable;
            }
            else
            {
                lastSize = *size;
                stable = 1;
            }
        }
        else
        {
            haveSize = false;
            stable = 0;
            lastSize = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    REQUIRE(stable >= 5);

    auto wavInputStream =
        std::make_shared<std::ifstream>(recordingPath, std::ios::binary);

    auto wavOrError = mpc::file::wav::WavFile::readWavStream(wavInputStream);

    REQUIRE(wavOrError.has_value());

    auto wav = wavOrError.value();
    std::vector<float> wavFrames(100);
    wav->readFrames(wavFrames, 100);

    for (int i = 0; i < 100; i++)
        REQUIRE(wavFrames[i] > 0.f);

    for (int i = 0; i < 10; ++i)
    {
        delete[] outputBuffer[i];
        if (i < 2)
            delete[] inputBuffer[i];
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
    constexpr int RUN_COUNT = 3;

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

        if (const auto dirItRes = mpc_fs::make_directory_iterator(recordingsPath))
        {
            for (auto entry = *dirItRes; entry != mpc_fs::directory_end(); ++entry)
            {
                if (mpc_fs::is_directory(*entry).value_or(false))
                {
                    recordingsPath = entry->path();
                    break;
                }
            }
        }

        auto recordingPath = recordingsPath / "L.wav";
        REQUIRE(mpc_fs::exists(recordingPath).value_or(false));

        std::uintmax_t lastSize = 0;
        int stable = 0;
        bool haveSize = false;

        for (int attempt = 0; attempt < 400 && stable < 5; ++attempt)
        {
            auto size = mpc_fs::file_size(recordingPath);
            if (size && *size > 44)
            {
                if (!haveSize)
                {
                    haveSize = true;
                    lastSize = *size;
                    stable = 1;
                }
                else if (*size == lastSize)
                {
                    ++stable;
                }
                else
                {
                    lastSize = *size;
                    stable = 1;
                }
            }
            else
            {
                haveSize = false;
                stable = 0;
                lastSize = 0;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        REQUIRE(stable >= 5);
        
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

        (void) mpc_fs::remove_all(recordingsPath);
        REQUIRE(!mpc_fs::exists(recordingsPath).value_or(false));
        return frames;
    };

    std::vector<std::vector<float>> allFrames;
    allFrames.reserve(RUN_COUNT);

    for (int run = 0; run < RUN_COUNT; ++run)
    {
        allFrames.push_back(runRecording());
    }

    REQUIRE(!allFrames.empty());

    const auto &reference = allFrames.front();

    for (int run = 1; run < RUN_COUNT; ++run)
    {
        REQUIRE(reference.size() == allFrames[run].size());
        for (size_t i = 0; i < reference.size(); ++i)
        {
            REQUIRE(reference[i] == allFrames[run][i]);
        }
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
