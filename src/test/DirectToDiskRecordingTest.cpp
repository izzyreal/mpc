#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "sequencer/Track.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "file/wav/WavFile.hpp"

#include <thread>

TEST_CASE("Direct to disk recording does not start with silence", "[direct-to-disk-recording]")
{
    const int BUFFER_SIZE = 512;
    const int DSP_CYCLE_DURATION_MICROSECONDS = 11601;
    const int DSP_CYCLE_COUNT = 2000000 / DSP_CYCLE_DURATION_MICROSECONDS;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sound = mpc.getSampler()->addSound();
    sound->setMono(true);

    for (int i = 0; i < 1000; i++)
    {
        sound->insertFrame( {1.f}, i);
    }

    sound->setStart(0);
    sound->setEnd(1000);

    mpc.getSampler()->getProgram(0)->getNoteParameters(35)->setSoundIndex(0);

    auto seq = mpc.getSequencer()->getActiveSequence();
    seq->init(1);
    seq->setInitialTempo(300);
    auto event = seq->getTrack(0)->recordNoteEventSynced(0, 35, 127);
    seq->getTrack(0)->finalizeNoteEventSynced(event, 1);

    mpc.getLayeredScreen()->openScreen("vmpc-direct-to-disk-recorder");

    auto audioMidiServices = mpc.getAudioMidiServices();
    auto audioServer = audioMidiServices->getAudioServer();

    audioServer->setSampleRate(44100);
    audioServer->resizeBuffers(BUFFER_SIZE);

    std::vector<float> outputBuffer(BUFFER_SIZE * 2);

    auto audioThread = std::thread([&]{
        for (int i = 0; i < DSP_CYCLE_COUNT; i++)
        {
            audioMidiServices->changeBounceStateIfRequired();
            audioServer->work(&outputBuffer[0], &outputBuffer[0], BUFFER_SIZE, 0, 2);
            std::this_thread::sleep_for(std::chrono::microseconds(DSP_CYCLE_DURATION_MICROSECONDS));
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mpc.getActiveControls()->function(4);

    const auto recordingsPath = mpc.paths->recordingsPath();
    const auto recordingPath = recordingsPath / "L.WAV";

    audioThread.join();

    REQUIRE(fs::exists(recordingPath));

    auto wavInputStream = std::make_shared<std::ifstream>(recordingPath);

    auto wavOrError = mpc::file::wav::WavFile::readWavStream(wavInputStream);

    assert(wavOrError.has_value());

    auto wav = wavOrError.value();
    std::vector<float> wavFrames(100);
    wav->readFrames(wavFrames, 100);

    for (int i = 0; i < 100; i++)
    {
        REQUIRE(wavFrames[i] > 0.002f);
    }
}