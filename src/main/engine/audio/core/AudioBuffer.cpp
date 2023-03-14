#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/ChannelFormat.hpp>

using namespace mpc::engine::audio::core;
using namespace std;

AudioBuffer::AudioBuffer(string name, int channelCount, int sampleCount, float sampleRate) : FloatSampleBuffer(channelCount, sampleCount, sampleRate)
{
	this->name = name;
	realTime = true;
	channelFormat = guessFormat();
}

const bool AudioBuffer::isSilent() {
    for (int c = 0; c < getChannelCount(); c++) {
        auto& data = getChannel(c);
        for (int s = 0; s < getSampleCount(); s++) {
            if (data[s] != 0) return false;
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
	if (count == getChannelCount()) return;

	if (count < getChannelCount()) {
		for (auto ch = getChannelCount() - 1; ch > count - 1; ch--) {
			removeChannel(ch);
		}
	}
	else {
		while (getChannelCount() < count) {
			addChannel(false);
		}
	}
}

bool AudioBuffer::isRealTime()
{
    return realTime;
}

void AudioBuffer::setRealTime(bool realTime)
{
    this->realTime = realTime;
}


shared_ptr<ChannelFormat> AudioBuffer::guessFormat()
{
    return ChannelFormat::STEREO();
}

shared_ptr<ChannelFormat> AudioBuffer::getChannelFormat()
{
    return channelFormat;
}

void AudioBuffer::setChannelFormat(shared_ptr<ChannelFormat> newFormat)
{
    auto l_newFormat = newFormat;
    auto l_channelFormat = channelFormat;

    if (l_newFormat == l_channelFormat) {
        return;
    }

	channelFormat = l_newFormat;

	if (l_newFormat) {
		setChannelCount(l_newFormat->getCount());
	}
}


void AudioBuffer::swap(int a, int b)
{
    auto ns = getSampleCount();
    auto& asamples = getChannel(a);
    auto& bsamples = getChannel(b);
    float tmp;
    for (auto s = 0; s < ns; s++) {
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

	for (auto c = 0; c < nc; c++) {
		auto& samples = getChannel(c);
		for (auto s = 0; s < ns; s++) {
			auto sample = samples[s];
			sumOfSquares += sample * sample;
		}
	}
	return sumOfSquares / (nc * ns);
}


void AudioBuffer::copyFrom(AudioBuffer* src)
{
    if(src == nullptr) return;

    setChannelFormat(src->getChannelFormat());
    auto nc = getChannelCount();
    auto ns = getSampleCount();
    for (auto c = 0; c < nc; c++) {
        auto& from = src->getChannel(c);
        auto& to = getChannel(c);
        for (auto s = 0; s < ns; s++) {
           to[s] = from[s];
        }
    }
}
