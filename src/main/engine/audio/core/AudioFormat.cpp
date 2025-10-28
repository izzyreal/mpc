#include "engine/audio/core/AudioFormat.hpp"

using namespace mpc::engine::audio::core;
using namespace std;

AudioFormat::AudioFormat(float sampleRate, int sampleSizeInBits, int channels,
                         bool signed_, bool bigEndian)
{

    encoding = signed_ ? Encoding::PCM_SIGNED() : Encoding::PCM_UNSIGNED();
    frameSize = (sampleSizeInBits + 7) / 8 * channels;
    this->sampleRate = sampleRate;
    this->frameRate = sampleRate;
    this->sampleSizeInBits = sampleSizeInBits;
    this->channels = channels;
    this->bigEndian = bigEndian;
}

AudioFormat::AudioFormat(Encoding *encoding, float sampleRate,
                         int sampleSizeInBits, int channels, int frameSize,
                         float frameRate, bool bigEndian)
{
    this->sampleRate = sampleRate;
    this->sampleSizeInBits = sampleSizeInBits;
    this->channels = channels;
    this->bigEndian = bigEndian;
    this->encoding = encoding;
    this->frameSize = frameSize;
    this->frameRate = frameRate;
}

int AudioFormat::getChannels()
{
    return channels;
}

Encoding *AudioFormat::getEncoding()
{
    return encoding;
}

float AudioFormat::getFrameRate()
{
    return frameRate;
}

int AudioFormat::getFrameSize()
{
    return frameSize;
}

float AudioFormat::getSampleRate()
{
    return sampleRate;
}

int AudioFormat::getSampleSizeInBits()
{
    return sampleSizeInBits;
}

bool AudioFormat::isBigEndian()
{
    return bigEndian;
}

bool AudioFormat::matches(AudioFormat *fmt)
{
    if (encoding != fmt->encoding || channels != fmt->channels ||
        sampleSizeInBits != fmt->sampleSizeInBits ||
        frameSize != fmt->frameSize)
    {
        return false;
    }
    if (sampleRate != NOT_SPECIFIED && fmt->sampleRate != NOT_SPECIFIED &&
        sampleRate != fmt->sampleRate)
    {
        return false;
    }
    if (frameRate != NOT_SPECIFIED && fmt->frameRate != NOT_SPECIFIED &&
        frameRate != fmt->frameRate)
    {
        return false;
    }
    if (sampleSizeInBits > 8)
    {
        return bigEndian == fmt->bigEndian;
    }
    return true;
}
