#include "engine/audio/core/FloatSampleTools.hpp"

#include "engine/audio/core/Encoding.hpp"
#include "engine/audio/core/AudioFormat.hpp"

#include <cstdio>
#include <cstdlib>

using namespace mpc::engine::audio::core;

void FloatSampleTools::checkSupportedSampleSize(int ssib, int channels,
                                                int frameSize)
{
    if ((ssib * channels) != frameSize * 8)
    {
        std::string description =
            "unsupported sample size: " + std::to_string(ssib) + " stored in " +
            std::to_string(frameSize / channels) + " bytes.";
        printf("ERROR: %s", description.c_str());
        return;
    }
}

int FloatSampleTools::getFormatType(AudioFormat *format)
{
    bool signed_ = format->getEncoding() == Encoding::PCM_SIGNED();
    if (!signed_ && format->getEncoding() != Encoding::PCM_UNSIGNED())
    {
        std::string description =
            "unsupported encoding: only PCM encoding supported.";
        printf("ERROR: %s", description.c_str());
        return -1;
    }
    if (!signed_ && format->getSampleSizeInBits() != 8)
    {
        std::string description =
            "unsupported encoding: only 8-bit can be unsigned";
        printf("ERROR: %s", description.c_str());
        return -1;
    }
    checkSupportedSampleSize(format->getSampleSizeInBits(),
                             format->getChannels(), format->getFrameSize());
    auto formatType = getFormatType(format->getSampleSizeInBits(), signed_,
                                    format->isBigEndian());
    return formatType;
}

int FloatSampleTools::getFormatType(int ssib, bool signed_, bool bigEndian)
{
    int bytesPerSample = ssib / 8;
    int res = 0;
    if (ssib == 8)
    {
        res = F_8;
    }
    else if (ssib == 16)
    {
        res = F_16;
    }
    else if (ssib == 24)
    {
        res = F_24;
    }
    else if (ssib == 32)
    {
        res = F_32;
    }
    if (res == 0)
    {
        std::string description =
            "FloatSampleBuffer: unsupported sample size of " +
            std::to_string(ssib) + " bits per sample.";
        printf("ERROR: %s", description.c_str());
    }
    if (!signed_ && bytesPerSample > 1)
    {
        std::string description =
            "FloatSampleBuffer: unsigned samples larger than 8 bit are not "
            "supported";
    }
    if (signed_)
    {
        res |= F_SIGNED;
    }
    if (bigEndian && (ssib != 8))
    {
        res |= F_BIGENDIAN;
    }
    return res;
}

int FloatSampleTools::getSampleSize(int formatType)
{
    switch (formatType & F_SAMPLE_WIDTH_MASK)
    {
        case F_8:
            return 1;
        case F_16:
            return 2;
        case F_24:
            return 3;
        case F_32:
            return 4;
    }
    return 0;
}

std::string FloatSampleTools::formatType2Str(int formatType)
{
    std::string result = std::to_string(formatType) + ": ";
    switch (formatType & F_SAMPLE_WIDTH_MASK)
    {
        case F_8:
            result = result.append("8bit");
            break;
        case F_16:
            result = result.append("16bit");
            break;
        case F_24:
            result = result.append("24bit");
            break;
        case F_32:
            result = result.append("32bit");
            break;
    }

    bool signed_ = (formatType & F_SIGNED) == F_SIGNED;
    result = result.append(signed_ ? " signed" : " unsigned");
    if ((formatType & F_SAMPLE_WIDTH_MASK) != F_8)
    {
        bool bigEndian = (formatType & F_BIGENDIAN) == F_BIGENDIAN;
        result = result.append(bigEndian ? " big endian" : " little endian");
    }
    return result;
}

const float FloatSampleTools::twoPower7 = 128.0f;
const float FloatSampleTools::twoPower15 = 32768.0f;
const float FloatSampleTools::twoPower23 = 8388608.0f;
const float FloatSampleTools::twoPower31 = 2.14748365E9f;
const float FloatSampleTools::invTwoPower7 = 1.f / twoPower7;
const float FloatSampleTools::invTwoPower15 = 1.f / twoPower15;
const float FloatSampleTools::invTwoPower23 = 1.f / twoPower23;
const float FloatSampleTools::invTwoPower31 = 1.f / twoPower31;

void FloatSampleTools::byte2float(const std::vector<char> &input,
                                  int inByteOffset,
                                  std::vector<std::vector<float>> &output,
                                  int outOffset, int frameCount,
                                  AudioFormat *format)
{
    for (auto channel = 0; channel < format->getChannels(); channel++)
    {
        if (output.size() < channel)
        {
            output.emplace_back(frameCount);
        }
        else
        {
            for (int i = 0; i < frameCount; i++)
            {
                output[channel][i] = 0;
            }
        }
        byte2floatGeneric(input, inByteOffset, format->getFrameSize(),
                          output[channel], outOffset, frameCount, format);
        inByteOffset += format->getFrameSize() / format->getChannels();
    }
}

void FloatSampleTools::byte2floatGeneric(const std::vector<char> &input,
                                         int inByteOffset, int inByteStep,
                                         std::vector<float> &output,
                                         int outOffset, int sampleCount,
                                         AudioFormat *format)
{
    auto formatType = getFormatType(format);
    byte2floatGeneric(input, inByteOffset, inByteStep, output, outOffset,
                      sampleCount, formatType);
}

void FloatSampleTools::byte2floatGeneric(const std::vector<char> &input,
                                         int inByteOffset, int inByteStep,
                                         std::vector<float> &output,
                                         int outOffset, int sampleCount,
                                         int formatType)
{
    auto endCount = outOffset + sampleCount;
    auto inIndex = inByteOffset;
    for (auto outIndex = outOffset; outIndex < endCount;
         outIndex++, inIndex += inByteStep)
    {
        switch (formatType)
        {
            case CT_8S:
                output[outIndex] = input[inIndex] * invTwoPower7;
                break;
            case CT_8U:
                output[outIndex] =
                    ((input[inIndex] & 255) - 128) * invTwoPower7;
                break;
            case CT_16SB:
                output[outIndex] =
                    ((input[inIndex] << 8) | (input[inIndex + 1] & 255)) *
                    invTwoPower15;
                break;
            case CT_16SL:
                output[outIndex] =
                    ((input[inIndex + 1] << 8) | (input[inIndex] & 255)) *
                    invTwoPower15;
                break;
            case CT_24SB:
                output[outIndex] = ((input[inIndex] << 16) |
                                    ((input[inIndex + 1] & 255) << 8) |
                                    (input[inIndex + 2] & 255)) *
                                   invTwoPower23;
                break;
            case CT_24SL:
                output[outIndex] = ((input[inIndex + 2] << 16) |
                                    ((input[inIndex + 1] & 255) << 8) |
                                    (input[inIndex] & 255)) *
                                   invTwoPower23;
                break;
            case CT_32SB:
                output[outIndex] = ((input[inIndex] << 24) |
                                    ((input[inIndex + 1] & 255) << 16) |
                                    ((input[inIndex + 2] & 255) << 8) |
                                    (input[inIndex + 3] & 255)) *
                                   invTwoPower31;
                break;
            case CT_32SL:
                output[outIndex] = ((input[inIndex + 3] << 24) |
                                    ((input[inIndex + 2] & 255) << 16) |
                                    ((input[inIndex + 1] & 255) << 8) |
                                    (input[inIndex] & 255)) *
                                   invTwoPower31;
                break;
            default:
                std::string description =
                    "unsupported format=" + formatType2Str(formatType);
                printf("ERROR: %s", description.c_str());
        }
    }
}

int8_t FloatSampleTools::quantize8(float sample, float ditherBits)
{
    if (ditherBits != 0)
    {
        // sample += npc(random_)->nextFloat() * ditherBits;
    }
    if (sample >= 127.0f)
    {
        return static_cast<int8_t>(127);
    }
    else if (sample <= -128.0f)
    {
        return static_cast<int8_t>(-128);
    }
    else
    {
        return static_cast<int8_t>(
            (sample < 0 ? (sample - 0.5f) : (sample + 0.5f)));
    }
}

int FloatSampleTools::quantize16(float sample, float ditherBits)
{
    if (ditherBits != 0)
    {
        sample += (((float)rand() / RAND_MAX) + 1) *
                  ditherBits; // apparently this is not super duper distributed
                              // random or something. check original.
    }
    if (sample >= 32767.0f)
    {
        return 32767;
    }
    else if (sample <= -32768.0f)
    {
        return -32768;
    }
    else
    {
        return static_cast<int>(sample < 0 ? (sample - 0.5f) : (sample + 0.5f));
    }
}

int FloatSampleTools::quantize24(float sample, float ditherBits)
{
    if (ditherBits != 0)
    {
        sample += (((float)rand() / RAND_MAX) + 1) * ditherBits;
    }
    if (sample >= 8388607.0f)
    {
        return 8388607;
    }
    else if (sample <= -8388608.0f)
    {
        return -8388608;
    }
    else
    {
        return static_cast<int>(sample < 0 ? (sample - 0.5f) : (sample + 0.5f));
    }
}

int FloatSampleTools::quantize32(float sample, float ditherBits)
{
    if (ditherBits != 0)
    {
        sample += (((float)rand() / RAND_MAX) + 1) * ditherBits;
    }
    if (sample >= 2.14748365E9f)
    {
        return 2147483647;
    }
    else if (sample <= -2.14748365E9f)
    {
        return int(-0x7fffffff - 1);
    }
    else
    {
        return static_cast<int>(
            (sample < 0 ? (sample - 0.5f) : (sample + 0.5f)));
    }
}

void FloatSampleTools::float2byte(std::vector<std::vector<float>> &input,
                                  int inOffset, std::vector<char> &output,
                                  int outByteOffset, int frameCount,
                                  AudioFormat *format, float ditherBits)
{
    for (auto channel = 0; channel < format->getChannels(); channel++)
    {
        auto data = input[channel];
        float2byteGeneric(data, inOffset, output, outByteOffset,
                          format->getFrameSize(), frameCount, format,
                          ditherBits);
        outByteOffset += format->getFrameSize() / format->getChannels();
    }
}

void FloatSampleTools::float2byteGeneric(std::vector<float> &input,
                                         int inOffset,
                                         std::vector<char> &output,
                                         int outByteOffset, int outByteStep,
                                         int sampleCount, AudioFormat *format,
                                         float ditherBits)
{
    int formatType = getFormatType(format);
    float2byteGeneric(input, inOffset, output, outByteOffset, outByteStep,
                      sampleCount, formatType, ditherBits);
}

void FloatSampleTools::float2byteGeneric(std::vector<float> &input,
                                         int inOffset,
                                         std::vector<char> &output,
                                         int outByteOffset, int outByteStep,
                                         int sampleCount, int formatType,
                                         float ditherBits)
{
    if (inOffset < 0 || inOffset + sampleCount > input.size() ||
        sampleCount < 0)
    {
        std::string description = "invalid input index: input.length=" +
                                  std::to_string(input.size()) + " inOffset" +
                                  std::to_string(inOffset) +
                                  " sampleCount=" + std::to_string(sampleCount);
        // TO-DO Throw description
        printf("ERROR: %s\n", description.c_str());
    }
    if (outByteOffset < 0 ||
        outByteOffset + (sampleCount * outByteStep) >=
            (output.size() + outByteStep) ||
        outByteStep < getSampleSize(formatType))
    {
        std::string description =
            "invalid output index: output.length= " +
            std::to_string(output.size()) +
            " outByteOffset=" + std::to_string(outByteOffset) +
            " outByteStep=" + std::to_string(outByteStep) +
            " sampleCount=" + std::to_string(sampleCount) +
            " format=" + formatType2Str(formatType);
        printf("ERROR: %s\n", description.c_str());
    }

    /*
    if(ditherBits != 0.0f && random_ == nullptr) {
            random_ = new ::java::util::Random();
    }
    */

    int endSample = inOffset + sampleCount;
    int iSample;
    int outIndex = outByteOffset;
    for (int inIndex = inOffset; inIndex < endSample;
         inIndex++, outIndex += outByteStep)
    {

        switch (formatType)
        {
            case CT_8S:
                output[outIndex] =
                    quantize8(input[inIndex] * twoPower7, ditherBits);
                break;
            case CT_8U:
                output[outIndex] =
                    quantize8((input[inIndex] * twoPower7), ditherBits) + 128;
                break;
            case CT_16SB:
                iSample = quantize16(input[inIndex] * twoPower15, ditherBits);
                output[outIndex] = iSample >> 8;
                output[outIndex + 1] = iSample & 255;
                break;
            case CT_16SL:
                iSample = quantize16(input[inIndex] * twoPower15, ditherBits);
                output[outIndex + 1] = (char)(iSample >> 8);
                output[outIndex] = (char)(iSample & 255);
                break;
            case CT_24SB:
                iSample = quantize24(input[inIndex] * twoPower23, ditherBits);
                output[outIndex] = iSample >> 16;
                output[outIndex + 1] =
                    static_cast<int>(static_cast<uint32_t>(iSample) >> 8) & 255;
                output[outIndex + 2] = iSample & 255;
                break;
            case CT_24SL:
                iSample = quantize24(input[inIndex] * twoPower23, ditherBits);
                output[outIndex + 2] = iSample >> 16;
                output[outIndex + 1] =
                    static_cast<int>(static_cast<uint32_t>(iSample) >> 8) & 255;
                output[outIndex] = iSample & 255;
                break;
            case CT_32SB:
                iSample = quantize32(input[inIndex] * twoPower31, ditherBits);
                output[outIndex] = iSample >> 24;
                output[outIndex + 1] =
                    static_cast<int>(static_cast<uint32_t>(iSample) >> 16) &
                    255;
                output[outIndex + 2] =
                    static_cast<int>(static_cast<uint32_t>(iSample) >> 8) & 255;
                output[outIndex + 3] = iSample & 255;
                break;
            case CT_32SL:
                iSample = quantize32(input[inIndex] * twoPower31, ditherBits);
                output[outIndex + 3] = iSample >> 24;
                output[outIndex + 2] =
                    static_cast<int>(static_cast<uint32_t>(iSample) >> 16) &
                    255;
                output[outIndex + 1] =
                    static_cast<int>(static_cast<uint32_t>(iSample) >> 8) & 255;
                output[outIndex] = iSample & 255;
                break;
            default:
                std::string description =
                    "unsupported format=" + formatType2Str(formatType);
                printf("ERROR: %s", description.c_str());
        }
    }
}
