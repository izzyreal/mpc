#include "engine/audio/server/AudioServer.hpp"

using namespace mpc::engine::audio::server;
using namespace mpc::engine::audio::core;

AudioServer::AudioServer() {}

float AudioServer::getSampleRate()
{
    return sampleRate;
}

void AudioServer::setSampleRate(int rate)
{
    sampleRate = static_cast<float>(rate);
    for (auto &b : buffers)
    {
        b->setSampleRate(sampleRate);
    }
}

const std::vector<std::shared_ptr<AudioBuffer>> &AudioServer::getBuffers()
{
    return buffers;
}

const unsigned int AudioServer::getBufferSize() const
{
    return bufferSize;
}

std::shared_ptr<AudioBuffer>
AudioServer::createAudioBuffer(const std::string name)
{
    buffers.emplace_back(
        std::make_shared<AudioBuffer>(name, 2, bufferSize, sampleRate));
    return buffers.back();
}

void AudioServer::resizeBuffers(int newSize)
{
    bufferSize = newSize;

    for (auto &b : buffers)
    {
        b->changeSampleCount(bufferSize, false);
    }
}

void AudioServer::removeAudioBuffer(std::shared_ptr<AudioBuffer> buffer)
{
    for (int i = 0; i < buffers.size(); i++)
    {
        if (buffers[i] == buffer)
        {
            buffers.erase(buffers.begin() + i);
            break;
        }
    }
}
