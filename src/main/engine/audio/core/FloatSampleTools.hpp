#pragma once
#include "engine/audio/core/AudioFormat.hpp"

#include <string>
#include <vector>

namespace mpc::engine::audio::core
{

    static float DEFAULT_DITHER_BITS = 0.7f;

    class FloatSampleTools
    {

    public:
        static const int F_8 = {int(1)};
        static const int F_16 = {int(2)};
        static const int F_24 = {int(3)};
        static const int F_32 = {int(4)};
        static const int F_SAMPLE_WIDTH_MASK = {int(7)};
        static const int F_SIGNED = {int(8)};
        static const int F_BIGENDIAN = {int(16)};
        static const int CT_8S = {int(9)};
        static const int CT_8U = {int(1)};
        static const int CT_16SB = {int(26)};
        static const int CT_16SL = {int(10)};
        static const int CT_24SB = {int(27)};
        static const int CT_24SL = {int(11)};
        static const int CT_32SB = {int(28)};
        static const int CT_32SL = {int(12)};

        static void checkSupportedSampleSize(int ssib, int channels,
                                             int frameSize);
        static int getFormatType(AudioFormat *format);
        static int getFormatType(int ssib, bool signed_, bool bigEndian);
        static int getSampleSize(int formatType);
        static std::string formatType2Str(int formatType);

    private:
        static const float twoPower7;
        static const float twoPower15;
        static const float twoPower23;
        static const float twoPower31;
        static const float invTwoPower7;
        static const float invTwoPower15;
        static const float invTwoPower23;
        static const float invTwoPower31;

    public:
        static void byte2float(const std::vector<char> &input, int inByteOffset,
                               std::vector<std::vector<float>> &output,
                               int outOffset, int frameCount,
                               AudioFormat *format);

        static void byte2floatGeneric(const std::vector<char> &input,
                                      int inByteOffset, int inByteStep,
                                      std::vector<float> &output, int outOffset,
                                      int sampleCount, AudioFormat *format);
        static void byte2floatGeneric(const std::vector<char> &input,
                                      int inByteOffset, int inByteStep,
                                      std::vector<float> &output, int outOffset,
                                      int sampleCount, int formatType);

    private:
        static int8_t quantize8(float sample, float ditherBits);
        static int quantize16(float sample, float ditherBits);
        static int quantize24(float sample, float ditherBits);
        static int quantize32(float sample, float ditherBits);

    public:
        static void float2byte(const std::vector<std::vector<float>> &input,
                               int inOffset, std::vector<char> &output,
                               int outByteOffset, int frameCount,
                               AudioFormat *format, float ditherBits);

        static void float2byteGeneric(std::vector<float> &input, int inOffset,
                                      std::vector<char> &output,
                                      int outByteOffset, int outByteStep,
                                      int sampleCount, AudioFormat *format,
                                      float ditherBits);
        static void float2byteGeneric(const std::vector<float> &input,
                                      int inOffset, std::vector<char> &output,
                                      int outByteOffset, int outByteStep,
                                      int sampleCount, int formatType,
                                      float ditherBits);
    };
} // namespace mpc::engine::audio::core
