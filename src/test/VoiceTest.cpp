#include "audiomidi/AudioMidiServices.hpp"
#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "sampler/Sampler.hpp"
#include "engine/Drum.hpp"

#include <chrono>
#include <thread>

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;

TEST_CASE("Voice", "[voice]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const int SOUND_FRAME_COUNT = 1;

    auto sound = mpc.getSampler()->addSound("foo");

    for (size_t idx = 0; idx < SOUND_FRAME_COUNT; idx++)
    {
        sound->insertFrame({0, 0}, 0);
    }
    
    sound->setEnd(sound->getFrameCount());

    mpc.getSampler()->getProgram(0)->getNoteParameters(35)->setSoundIndex(0);

    const int BUFFER_SIZE = 32;
    const int SAMPLE_RATE = 44100;
    const int DSP_CYCLE_DURATION_MICROSECONDS = 725 / 100;
    const int DSP_CYCLE_COUNT = 200000000 / DSP_CYCLE_DURATION_MICROSECONDS;

    auto audioMidiServices = mpc.getAudioMidiServices();
    auto audioServer = audioMidiServices->getAudioServer();
    audioServer->setSampleRate(SAMPLE_RATE);
    audioServer->resizeBuffers(BUFFER_SIZE);

    const float** inputBuffer = new const float*[2];
    inputBuffer[0] = new float[BUFFER_SIZE];
    inputBuffer[1] = new float[BUFFER_SIZE];

    float** outputBuffer = new float*[10];

    for (int i = 0; i < 10; ++i)
    {
        outputBuffer[i] = new float[BUFFER_SIZE];
    }

    std::function<void()> audioThreadFunction = [&](){
        for (int i = 0; i < DSP_CYCLE_COUNT; i++)
        {
            audioServer->work(inputBuffer, outputBuffer, BUFFER_SIZE, {}, {0, 1}, {}, {0, 1});
            std::this_thread::sleep_for(std::chrono::microseconds(DSP_CYCLE_DURATION_MICROSECONDS));
        }
    };

    auto audioThread = std::thread(audioThreadFunction);

    for (int i = 0; i < DSP_CYCLE_COUNT; i++)
    {
        mpc.getDrum(0).mpcNoteOn(35, 127, 0, 0, 0, true, 0, sound->getFrameCount());
        std::this_thread::sleep_for(std::chrono::microseconds(DSP_CYCLE_DURATION_MICROSECONDS));
    }

    audioThread.join();
}

