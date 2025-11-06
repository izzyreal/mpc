#include "engine/audio/core/AudioBuffer.hpp"

using namespace mpc::engine::audio::core;
using namespace std;

AudioBuffer::AudioBuffer(const string &name, int channelCount, int sampleCount,
                         float sampleRate)
    : FloatSampleBuffer(channelCount, sampleCount, sampleRate)
{
    this->name = name;
    realTime = true;
}

const bool AudioBuffer::isSilent()
{
    for (int c = 0; c < getChannelCount(); c++)
    {
        auto &data = getChannel(c);
        for (int s = 0; s < getSampleCount(); s++)
        {
            if (data[s] != 0)
            {
                return false;
            }
        }
    }
    return true;
}

string AudioBuffer::getName()
{
    return name;
}

void AudioBuffer::setChannelCount(int count)
{
    if (count == getChannelCount())
    {
        return;
    }

    if (count < getChannelCount())
    {
        for (auto ch = getChannelCount() - 1; ch > count - 1; ch--)
        {
            removeChannel(ch);
        }
    }
    else
    {
        while (getChannelCount() < count)
        {
            addChannel(false);
        }
    }
}

bool AudioBuffer::isRealTime() const
{
    return realTime;
}

void AudioBuffer::setRealTime(bool realTime)
{
    this->realTime = realTime;
}

void AudioBuffer::swap(int a, int b)
{
    auto ns = getSampleCount();
    auto &asamples = getChannel(a);
    auto &bsamples = getChannel(b);
    float tmp;
    for (auto s = 0; s < ns; s++)
    {
        tmp = asamples[s];
        asamples[s] = bsamples[s];
        bsamples[s] = tmp;
    }
}

float AudioBuffer::square()
{
    auto ns = getSampleCount();
    auto nc = getChannelCount();
    auto sumOfSquares = 0.0f;

    for (auto c = 0; c < nc; c++)
    {
        auto &samples = getChannel(c);
        for (auto s = 0; s < ns; s++)
        {
            auto sample = samples[s];
            sumOfSquares += sample * sample;
        }
    }
    return sumOfSquares / (nc * ns);
}

void AudioBuffer::copyFrom(AudioBuffer *src)
{
    if (src == nullptr)
    {
        return;
    }
    auto nc = getChannelCount();
    auto ns = getSampleCount();
    for (auto c = 0; c < nc; c++)
    {
        auto &from = src->getChannel(c);
        auto &to = getChannel(c);
        for (auto s = 0; s < ns; s++)
        {
            to[s] = from[s];
        }
    }
}
