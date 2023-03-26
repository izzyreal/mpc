#include "FloatSampleBuffer.hpp"

#include <stdexcept>

using namespace mpc::engine::audio::core;

FloatSampleBuffer::FloatSampleBuffer()
{
	init(0, 0, 1);
}

FloatSampleBuffer::FloatSampleBuffer(int channelCount, int sampleCount, float sampleRate)
{
	init(channelCount, sampleCount, sampleRate, LAZY_DEFAULT);
}

void FloatSampleBuffer::init(int channelCountToUse, int sampleCountToUse, float sampleRateToUse)
{
    init(channelCountToUse, sampleCountToUse, sampleRateToUse, LAZY_DEFAULT);
}

void FloatSampleBuffer::init(int channelCountToUse, int sampleCountToUse, float sampleRateToUse, bool lazy)
{
	if (channelCountToUse < 0 || sampleCountToUse < 0) {
		std::string error = "invalid parameters in initialization of FloatSampleBuffer.";
		printf("ERROR: %s\n", error.c_str());
		return;
	}
	setSampleRate(sampleRateToUse);
	if (getSampleCount() != sampleCountToUse || getChannelCount() != channelCountToUse) {
		createChannels(channelCountToUse, sampleCountToUse, lazy);
	}
}

void FloatSampleBuffer::createChannels(int channelCountToUse, int sampleCountToUse, bool lazy)
{
	this->sampleCount = sampleCountToUse;
	this->channelCount = 0;
	channels.clear();
	for (auto ch = 0; ch < channelCountToUse; ch++) {
		insertChannel(ch, false, lazy);
	}
	if (!lazy) {
		while (channels.size() > channelCountToUse) {
			channels.erase(channels.begin() + (int) (channels.size()) - 1);
		}
	}
}

void FloatSampleBuffer::initFromByteArray_(std::vector<char>& buffer, int offset, int byteCount, AudioFormat* format)
{
	initFromByteArray_(buffer, offset, byteCount, format, LAZY_DEFAULT);
}

void FloatSampleBuffer::initFromByteArray_(std::vector<char>& buffer, int offset, int byteCount, AudioFormat* format, bool lazy)
{
	if (offset + byteCount > buffer.size()) {
		return;
	}
	auto thisSampleCount = byteCount / format->getFrameSize();
	init(format->getChannels(), thisSampleCount, format->getSampleRate(), lazy);
	originalFormatType = FloatSampleTools::getFormatType(format);
	FloatSampleTools::byte2float(buffer, offset, channels, 0, sampleCount, format);
}


void FloatSampleBuffer::reset()
{
    init(0, 0, 1, false);
}

void FloatSampleBuffer::reset(int channelsToUse, int sampleCountToUse, float sampleRateToUse)
{
	init(channelsToUse, sampleCountToUse, sampleRateToUse, false);
}

int FloatSampleBuffer::getByteArrayBufferSize(AudioFormat* format)
{
    return FloatSampleBuffer::getByteArrayBufferSize(format, getSampleCount());
}

int FloatSampleBuffer::getByteArrayBufferSize(AudioFormat* format, int lenInSamples)
{
    return format->getFrameSize() * lenInSamples;
}

int FloatSampleBuffer::convertToByteArray_(int readOffset, int lenInSamples, std::vector<char>& buffer, int writeOffset, AudioFormat* format)
{
	int byteCount = getByteArrayBufferSize(format, lenInSamples);
	if (writeOffset + byteCount > buffer.size()) {
		std::string error = "FloatSampleBuffer.convertToByteArray: buffer too small.";
		printf("ERROR: %s\n", error.c_str());
		return -1;
	}
	if (format->getSampleRate() != getSampleRate()) {
        std::string error = "FloatSampleBuffer.convertToByteArray: different samplerates.";
		printf("ERROR: %s\n", error.c_str());
		return -1;
	}
	if (format->getChannels() != getChannelCount()) {
        std::string error = "FloatSampleBuffer.convertToByteArray: different channel count.";
		printf("ERROR: %s\n", error.c_str());
		return -1;
	}
	FloatSampleTools::float2byte(channels, readOffset, buffer, writeOffset, lenInSamples, format, getConvertDitherBits(FloatSampleTools::getFormatType(format)));
	return byteCount;
}

void FloatSampleBuffer::changeSampleCount(unsigned int newSampleCount)
{
	if (getSampleCount() == newSampleCount)
    {
		return;
	}

	init(getChannelCount(), newSampleCount, getSampleRate());
}

void FloatSampleBuffer::makeSilence()
{
    for (int ch = 0; ch < getChannelCount(); ch++)
    {
        makeSilence(ch);
    }
}

void FloatSampleBuffer::makeSilence(int channel)
{
	getChannel(channel) = 0;
}

void FloatSampleBuffer::addChannel(bool silent)
{
    insertChannel(getChannelCount(), silent);
}

void FloatSampleBuffer::insertChannel(int index, bool silent)
{
    insertChannel(index, silent, LAZY_DEFAULT);
}

void FloatSampleBuffer::insertChannel(int index, bool silent, bool lazy)
{
	int physSize = static_cast<int>(channels.size());
	int virtSize = getChannelCount();
	std::valarray<float> newChannel;

	if (physSize > virtSize)
    {
		for (int ch = virtSize; ch < physSize; ch++)
        {
			auto thisChannel = channels[ch];

            if ((lazy && thisChannel.size() >= getSampleCount()) || (!lazy && thisChannel.size() == getSampleCount()))
            {
				newChannel = thisChannel;
				channels.erase(channels.begin() + ch);
				break;
			}
		}
	}

	if (newChannel.size() == 0)
    {
        newChannel.resize(getSampleCount());
        newChannel = 0;
	}

    channels.emplace_back(newChannel);
	channelCount++;

    if (silent)
    {
        makeSilence(index);
    }
}

void FloatSampleBuffer::removeChannel(int channel)
{
    removeChannel(channel, LAZY_DEFAULT);
}

void FloatSampleBuffer::removeChannel(int channel, bool lazy)
{
	if (!lazy)
    {
		channels.erase(channels.begin() + channel);
	}
	else if (channel < getChannelCount() - 1)
    {
		std::valarray<float> candidate = channels[channel];
		channels.erase(channels.begin() + channel);
		channels.emplace_back(candidate);
	}

    channelCount--;
}

void FloatSampleBuffer::copyChannel(int sourceChannel, int targetChannel)
{
	getChannel(targetChannel) = getChannel(sourceChannel);
}

void FloatSampleBuffer::copy(int sourceIndex, int destIndex, int length)
{
	for (auto i = 0; i < getChannelCount(); i++)
		copy(i, sourceIndex, destIndex, length);
}

void FloatSampleBuffer::copy(int channel, int sourceIndex, int destIndex, int length)
{
	auto& data = getChannel(channel);
	auto bufferCount = getSampleCount();

	if (sourceIndex + length > bufferCount || destIndex + length > bufferCount || sourceIndex < 0 || destIndex < 0 || length < 0)
    {
		std::string error = "parameters exceed buffer size";
		printf("ERROR: %s\n", error.c_str());
	}

    for (int i = 0; i < length; i++)
		data[destIndex + i] = data[sourceIndex + i];
}

int FloatSampleBuffer::getChannelCount() const
{
	return channelCount;
}

int FloatSampleBuffer::getSampleCount() const
{
	return sampleCount;
}

float FloatSampleBuffer::getSampleRate() const
{
    return sampleRate;
}
void FloatSampleBuffer::setSampleRate(float sampleRateToUse)
{
	if (sampleRateToUse <= 0)
    {
		std::string error = "Invalid samplerate for FloatSampleBuffer.";
		printf("ERROR: %s\n", error.c_str());
		return;
	}

    sampleRate = sampleRateToUse;
}

std::valarray<float>& FloatSampleBuffer::getChannel(int channel)
{
	if (channel < 0 || channel >= getChannelCount())
    {
		std::string error = "FloatSampleBuffer: invalid channel index " + std::to_string(channel) + " was provided, only up to index " + std::to_string(channels.size() - 1) + " available.";
		printf("ERROR: %s\n", error.c_str());
		throw std::invalid_argument(error);
	}
	
    return channels[channel];
}

float FloatSampleBuffer::getConvertDitherBits(int newFormatType)
{
	auto doDither = false;
    
	switch (ditherMode)
    {
	case DITHER_MODE_AUTOMATIC:
		doDither = (originalFormatType & FloatSampleTools::F_SAMPLE_WIDTH_MASK) > (newFormatType & FloatSampleTools::F_SAMPLE_WIDTH_MASK);
		break;
	case DITHER_MODE_ON:
		doDither = true;
		break;
	case DITHER_MODE_OFF:
		doDither = false;
		break;
	}
    
	return doDither ? ditherBits : 0.0f;
}
