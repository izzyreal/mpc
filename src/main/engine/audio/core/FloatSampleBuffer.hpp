#pragma once
#include <vector>
#include "engine/audio/core/AudioFormat.hpp"
#include "engine/audio/core/FloatSampleTools.hpp"

namespace mpc::engine::audio::core
{
    class FloatSampleBuffer
    {
    public:
        FloatSampleBuffer();
        FloatSampleBuffer(int channelCount, int sampleCount, float sampleRate);

        float getConvertDitherBits(int newFormatType);

        static const int DITHER_MODE_AUTOMATIC = 0;
        static const int DITHER_MODE_ON = 1;
        static const int DITHER_MODE_OFF = 2;

    private:
        std::vector<std::vector<float>> channels;
        int sampleCount = 0;
        float sampleRate = 0;
        int originalFormatType = 0;
        static const bool LAZY_DEFAULT = true;
        int channelCount = 0;

        float ditherBits = DEFAULT_DITHER_BITS;
        int ditherMode = DITHER_MODE_AUTOMATIC;

        void createChannels(int channelCount, int sampleCount, bool lazy);

    protected:
        void init(int channelCount, int sampleCount, float sampleRate);
        void init(int channelCount, int sampleCount, float sampleRate,
                  bool lazy);

    public:
        void insertChannel(int index, bool silent);
        void insertChannel(int index, bool silent, bool lazy);
        void initFromByteArray_(const std::vector<char> &buffer, int offset,
                                int byteCount, AudioFormat *format);
        void initFromByteArray_(const std::vector<char> &buffer, int offset,
                                int byteCount, AudioFormat *format, bool lazy);

        void reset();
        void reset(int channels, int sampleCount, float sampleRate);
        int getByteArrayBufferSize(AudioFormat *format);
        int getByteArrayBufferSize(AudioFormat *format, int lenInSamples);

        int convertToByteArray_(int readOffset, int lenInSamples,
                                std::vector<char> &buffer, int writeOffset,
                                AudioFormat *format);

        void changeSampleCount(int newSampleCount, bool keepOldSamples);
        void makeSilence();
        void makeSilence(int channel);
        void addChannel(bool silent);

        void copyChannel(int sourceChannel, int targetChannel);
        void copy(int sourceIndex, int destIndex, int length);
        void copy(int channel, int sourceIndex, int destIndex, int length);
        void removeChannel(int channel);
        void removeChannel(int channel, bool lazy);

        int getChannelCount();
        std::vector<float> &getChannel(int channel);

        int getSampleCount();

        void setSampleRate(float sampleRate);
        float getSampleRate();
    };
} // namespace mpc::engine::audio::core
