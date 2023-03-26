#include <engine/audio/core/AudioBuffer.hpp>
#include <utility>

using namespace mpc::engine::audio::core;

AudioBuffer::AudioBuffer(std::string nameToUse, int channelCount, int sampleCount, float sampleRate)
: FloatSampleBuffer(channelCount, sampleCount, sampleRate)
{
    name = std::move(nameToUse);
	realTime = true;
}

bool AudioBuffer::isSilent()
{
    for (int c = 0; c < getChannelCount(); c++)
    {
        if (getChannel(c).sum() != 0)
        {
            return false;
        }
    }

    return true;
}

std::string AudioBuffer::getName()
{
   return name;
}

bool AudioBuffer::isRealTime()
{
    return realTime;
}

void AudioBuffer::setRealTime(bool realTime)
{
    this->realTime = realTime;
}

float AudioBuffer::square()
{
	auto ns = getSampleCount();
	auto nc = getChannelCount();
	auto sumOfSquares = 0.0f;

	for (auto c = 0; c < nc; c++) {
        sumOfSquares += (getChannel(c) * getChannel(c)).sum();
	}
	return sumOfSquares / (nc * ns);
}

void AudioBuffer::copyFrom(AudioBuffer* src)
{
    for (auto c = 0; c < getChannelCount(); c++) {
        getChannel(c) = src->getChannel(c);
    }
}
