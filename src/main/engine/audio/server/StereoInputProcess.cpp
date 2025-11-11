#include "engine/audio/server/StereoInputProcess.hpp"

#include "engine/audio/core/AudioBuffer.hpp"

using namespace mpc::engine::audio::server;
using namespace std;

StereoInputProcess::StereoInputProcess(const string &name)
    : AudioServerProcess(name)
{
}

int StereoInputProcess::processAudio(core::AudioBuffer *buffer, int nFrames)
{
    if (!buffer->isRealTime())
    {
        return AUDIO_DISCONNECT;
    }

    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);
    auto ns = nFrames <= localBuffer.size() ? nFrames : localBuffer.size();

    int sampleCounter = 0;

    for (int i = 0; i < ns; i++)
    {
        left[i] = localBuffer[sampleCounter++];
        right[i] = localBuffer[sampleCounter++];
    }

    return AUDIO_OK;
}
