#include "engine/audio/server/StereoOutputProcess.hpp"

#include "engine/audio/core/AudioBuffer.hpp"

using namespace mpc::engine::audio::server;
using namespace std;

StereoOutputProcess::StereoOutputProcess(string name) : AudioServerProcess(name)
{
}

int StereoOutputProcess::processAudio(
    mpc::engine::audio::core::AudioBuffer *buffer, int nFrames)
{

    if (!buffer->isRealTime())
    {
        return AudioProcess::AUDIO_OK;
    }

    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);

    int frameCounter = 0;

    for (int i = 0; i < nFrames * 2; i += 2)
    {
        localBuffer[i] = left[frameCounter];
        localBuffer[i + 1] = right[frameCounter++];
    }

    return AudioProcess::AUDIO_OK;
}
