#include "audiomidi/SoundPlayer.hpp"

#include "engine/audio/core/AudioFormat.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

#include "SampleOps.hpp"

#include "WavInputFileStream.hpp"
#include "SndInputFileStream.hpp"

#include <readerwriterqueue.h>

#include <algorithm>
#include <cstring>

using namespace mpc::sampler;
using namespace mpc::audiomidi;

SoundPlayer::SoundPlayer() : fileFormat()
{
    srcLeft = src_new(0, 1, &srcLeftError);
    srcRight = src_new(0, 1, &srcRightError);
    bufferLeft = std::make_shared<moodycamel::ReaderWriterQueue<float>>(60000);
    bufferRight = std::make_shared<moodycamel::ReaderWriterQueue<float>>(60000);
}

SoundPlayer::~SoundPlayer()
{
    playing.store(false);

    if (readThread.joinable())
    {
        readThread.join();
    }

    src_delete(srcLeft);
    src_delete(srcRight);
}

bool SoundPlayer::start(const std::shared_ptr<std::istream> &streamToUse,
                        const SoundPlayerFileFormat fileFormatToUse,
                        const int audioServerSampleRate)
{
    if (playing.load())
    {
        return false;
    }

    if (readThread.joinable())
    {
        readThread.join();
    }

    if (!streamToUse)
    {
        return false;
    }

    stream = streamToUse;
    fileFormat = fileFormatToUse;

    int validBits;
    int sourceSampleRate;
    int sourceNumChannels;

    bool valid = false;

    if (fileFormat == WAV)
    {
        valid = wav_read_header(stream, sourceSampleRate, validBits,
                                sourceNumChannels, sourceFrameCount,
                                wavSamplesAreFloat32);
    }
    else if (fileFormat == SND)
    {
        valid = snd_read_header(stream, sourceSampleRate, validBits,
                                sourceNumChannels, sourceFrameCount,
                                sndInputEncoding);
        wavSamplesAreFloat32 = false;
    }

    if (!valid)
    {
        return false;
    }

    inputAudioFormat = std::make_shared<AudioFormat>(
        static_cast<float>(sourceSampleRate), validBits, sourceNumChannels,
        true, false);

    src_reset(srcLeft);
    src_reset(srcRight);

    while (bufferLeft->pop())
    {
    }
    while (bufferRight->pop())
    {
    }

    const auto ratio = static_cast<float>(audioServerSampleRate) /
                       static_cast<float>(sourceSampleRate);

    if (ratio != 1.f)
    {
        resampleInputBufferLeft = std::vector<float>(60000);
        resampleInputBufferRight = std::vector<float>(60000);
        resampleOutputBuffer = std::vector<float>(60000);
    }

    totalResamplerGeneratedFrameCount.store(0);
    resamplerGeneratedFrameCounter = 0;

    fadeFactor = 1.0f;
    stopEarly.store(false);
    playedFrameCount = 0;
    ingestedSourceFrameCount = 0;
    mpc60SampleDecoder = mpc::file::kaitai::Mpc60SampleDecoder();
    mpc60HasPendingSample = false;
    mpc60PendingSample = 0.0f;
    mpc60DecodedSampleIndex = 0;

    playing.store(true);

    readThread = std::thread(
        [this, ratio]
        {
            while (playing.load() &&
                   ingestedSourceFrameCount < sourceFrameCount)
            {
                if (ratio == 1.f)
                {
                    readWithoutResampling();
                }
                else
                {
                    readWithResampling(ratio);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }

            stream.reset();
            resampleInputBufferLeft.clear();
            resampleInputBufferRight.clear();
            resampleOutputBuffer.clear();
        });

    return true;
}

void SoundPlayer::enableStopEarly()
{
    if (!playing.load())
    {
        return;
    }
    stopEarly.store(true);
}

void SoundPlayer::readWithoutResampling()
{
    const auto capacity = bufferLeft->max_capacity();
    const auto channels = inputAudioFormat->getChannels();

    const auto currentBufferSpace =
        capacity -
        std::max<size_t>(bufferLeft->size_approx(), bufferRight->size_approx());

    if (currentBufferSpace == 0 || ingestedSourceFrameCount >= sourceFrameCount)
    {
        return;
    }

    auto frameCountToIngest = std::min<int>(
        currentBufferSpace, sourceFrameCount - ingestedSourceFrameCount);

    frameCountToIngest = std::min<int>(frameCountToIngest, 10000);
    const int bytesPerSample = inputAudioFormat->getSampleSizeInBits() / 8;
    const auto byteCountToIngest =
        frameCountToIngest * inputAudioFormat->getFrameSize();

    for (int currentByteIndex = 0; currentByteIndex < byteCountToIngest;
         currentByteIndex += bytesPerSample)
    {
        if (channels == 2 && fileFormat == SND &&
            currentByteIndex >= byteCountToIngest / bytesPerSample)
        {
            break;
        }

        bufferLeft->emplace(readNextFrame());

        if (channels == 2 && fileFormat == WAV)
        {
            bufferRight->emplace(readNextFrame());
            currentByteIndex += bytesPerSample;
        }
    }

    if (channels == 2 && fileFormat == SND)
    {
        const auto bytesPerChannel = sourceFrameCount * bytesPerSample;

        stream->seekg(-(byteCountToIngest / 2) + bytesPerChannel,
                      std::ios_base::cur);

        for (int currentByteIndex = 0; currentByteIndex < byteCountToIngest / 2;
             currentByteIndex += bytesPerSample)
        {
            bufferRight->emplace(readNextFrame());
        }

        stream->seekg(-bytesPerChannel, std::ios_base::cur);
    }

    ingestedSourceFrameCount += frameCountToIngest;
}

void SoundPlayer::readWithResampling(const float ratio)
{
    const auto capacity = bufferLeft->max_capacity();
    const auto channels = inputAudioFormat->getChannels();

    const auto currentBufferSpace =
        capacity -
        std::max<size_t>(bufferLeft->size_approx(), bufferRight->size_approx());

    if (currentBufferSpace == 0 || ingestedSourceFrameCount >= sourceFrameCount)
    {
        return;
    }

    auto unresampledFrameCountToIngest = std::min<int>(
        currentBufferSpace, sourceFrameCount - ingestedSourceFrameCount);

    unresampledFrameCountToIngest =
        std::min<int>(unresampledFrameCountToIngest, 10000);

    const auto byteCountToIngest =
        unresampledFrameCountToIngest * inputAudioFormat->getFrameSize();

    int frameCounter = 0;
    const int bytesPerSample = inputAudioFormat->getSampleSizeInBits() / 8;

    for (int currentByteIndex = 0; currentByteIndex < byteCountToIngest;
         currentByteIndex += bytesPerSample)
    {
        if (channels == 2 && fileFormat == SND &&
            currentByteIndex >= byteCountToIngest / 2)
        {
            break;
        }

        resampleInputBufferLeft[frameCounter] = readNextFrame();

        if (channels == 2 && fileFormat == WAV)
        {
            resampleInputBufferRight[frameCounter] = readNextFrame();
            currentByteIndex += bytesPerSample;
        }

        frameCounter++;
    }

    if (channels == 2 && fileFormat == SND)
    {
        const auto bytesPerChannel = sourceFrameCount * 2;

        stream->seekg(-(byteCountToIngest / 2) + bytesPerChannel,
                      std::ios_base::cur);

        frameCounter = 0;

        for (int currentByteIndex = 0; currentByteIndex < byteCountToIngest / 2;
             currentByteIndex += bytesPerSample)
        {
            resampleInputBufferRight[frameCounter++] = readNextFrame();
        }

        stream->seekg(-bytesPerChannel, std::ios_base::cur);
    }

    ingestedSourceFrameCount += unresampledFrameCountToIngest;

    const int endOfInput = ingestedSourceFrameCount >= sourceFrameCount ? 1 : 0;

    SRC_DATA data;
    data.data_in = &resampleInputBufferLeft[0];
    data.input_frames = unresampledFrameCountToIngest;
    data.data_out = &resampleOutputBuffer[0];
    data.output_frames = resampleOutputBuffer.size();
    data.end_of_input = endOfInput;
    data.src_ratio = ratio;

    src_process(srcLeft, &data);

    for (int f = 0; f < data.output_frames_gen; f++)
    {
        bufferLeft->enqueue(resampleOutputBuffer[f]);
    }

    if (channels == 2)
    {
        data.data_in = &resampleInputBufferRight[0];
        src_process(srcRight, &data);

        for (int f = 0; f < data.output_frames_gen; f++)
        {
            bufferRight->enqueue(resampleOutputBuffer[f]);
        }
    }

    resamplerGeneratedFrameCounter += data.output_frames_gen;

    if (data.end_of_input == 1)
    {
        totalResamplerGeneratedFrameCount.store(resamplerGeneratedFrameCounter);
    }
}

float SoundPlayer::readNextFrame()
{
    if (fileFormat == SND &&
        sndInputEncoding == SndInputEncoding::Mpc60Packed12)
    {
        return readNextMpc60Frame();
    }

    if (inputAudioFormat->getSampleSizeInBits() == 24)
    {
        return sampleops::int24_to_float(readNext24BitInt());
    }

    if (inputAudioFormat->getSampleSizeInBits() == 32)
    {
        return wavSamplesAreFloat32 ? readNextFloat32()
                                    : static_cast<float>(readNextInt32()) /
                                          2147483648.0f;
    }

    return sampleops::short_to_float(readNextShort());
}

float SoundPlayer::readNextMpc60Frame()
{
    if (mpc60HasPendingSample)
    {
        mpc60HasPendingSample = false;
        ++mpc60DecodedSampleIndex;
        return mpc60PendingSample;
    }

    char buffer[3];
    stream->read(buffer, 3);

    if (stream->gcount() != 3)
    {
        return 0.0f;
    }

    const auto byte0 = static_cast<uint8_t>(buffer[0]);
    const auto byte1 = static_cast<uint8_t>(buffer[1]);
    const auto byte2 = static_cast<uint8_t>(buffer[2]);
    const auto sample0 = static_cast<uint16_t>(
        byte0 | ((byte1 & 0x0fU) << 8U));
    const auto sample1 = static_cast<uint16_t>(
        (byte2 << 4U) | ((byte1 & 0xf0U) >> 4U));

    const auto decodedSample0 =
        mpc60SampleDecoder.decodeImportedFloat(sample0, false);
    mpc60PendingSample =
        mpc60SampleDecoder.decodeImportedFloat(sample1, true);
    mpc60HasPendingSample = true;

    ++mpc60DecodedSampleIndex;
    return decodedSample0;
}

int32_t SoundPlayer::readNext24BitInt() const
{
    char buffer[3];
    stream->read(buffer, 3);

    if (stream->gcount() != 3)
    {
        return 0;
    }

    int32_t value = static_cast<unsigned char>(buffer[0]) |
                    static_cast<unsigned char>(buffer[1]) << 8 |
                    static_cast<unsigned char>(buffer[2]) << 16;

    if (value & 0x00800000)
    {
        value |= static_cast<int32_t>(0xFF000000);
    }

    return value;
}

int32_t SoundPlayer::readNextInt32() const
{
    char buffer[4];
    stream->read(buffer, 4);

    if (stream->gcount() != 4)
    {
        return 0;
    }

    return static_cast<int32_t>(static_cast<unsigned char>(buffer[0]) |
                                static_cast<unsigned char>(buffer[1]) << 8 |
                                static_cast<unsigned char>(buffer[2]) << 16 |
                                static_cast<unsigned char>(buffer[3]) << 24);
}

float SoundPlayer::readNextFloat32() const
{
    const auto bits = static_cast<uint32_t>(readNextInt32());
    float value = 0.0f;
    std::memcpy(&value, &bits, sizeof(value));
    return std::clamp(value, -1.0f, 1.0f);
}

short SoundPlayer::readNextShort() const
{
    char buffer[2];
    stream->read(buffer, 2);

    if (stream->gcount() != 2)
    {
        return 0;
    }

    const short value = static_cast<unsigned char>(buffer[0]) |
                        static_cast<unsigned char>(buffer[1]) << 8;

    return value;
}

int SoundPlayer::processAudio(AudioBuffer *buf, const int nFrames)
{
    if (!playing.load())
    {
        buf->makeSilence();
        return AUDIO_SILENCE;
    }

    size_t availableFrameCount = bufferLeft->size_approx();

    if (inputAudioFormat->getChannels() == 2)
    {
        availableFrameCount =
            std::min<size_t>(availableFrameCount, bufferRight->size_approx());
    }

    int offsetWithinBuffer = 0;
    int lastFrameIndexWithinBuffer = nFrames;

    if (availableFrameCount < nFrames)
    {
        buf->makeSilence();

        if (playedFrameCount == 0)
        {
            offsetWithinBuffer = nFrames - availableFrameCount;
        }
        else
        {
            lastFrameIndexWithinBuffer = availableFrameCount;
        }
    }

    auto &outputBufferLeft = buf->getChannel(0);
    auto &outputBufferRight = buf->getChannel(1);

    for (int frame = offsetWithinBuffer; frame < lastFrameIndexWithinBuffer;
         frame++)
    {
        outputBufferLeft[frame] = *bufferLeft->peek();
        bufferLeft->pop();

        if (inputAudioFormat->getChannels() == 2)
        {
            outputBufferRight[frame] = *bufferRight->peek();
            bufferRight->pop();
        }
        else
        {
            outputBufferRight[frame] = outputBufferLeft[frame];
        }

        if (stopEarly.load())
        {
            if (fadeFactor > 0.f)
            {
                fadeFactor -= 0.002f;
            }
            else
            {
                playing.store(false);
            }

            outputBufferLeft[frame] *= fadeFactor;
            outputBufferRight[frame] *= fadeFactor;
        }

        playedFrameCount++;
    }

    if (const auto ratio =
            buf->getSampleRate() / inputAudioFormat->getSampleRate();
        ratio == 1.f)
    {
        if (playedFrameCount >= sourceFrameCount)
        {
            playing.store(false);
        }
    }
    else
    {
        const auto generatedFrameCount =
            totalResamplerGeneratedFrameCount.load();

        if (generatedFrameCount != 0 && playedFrameCount >= generatedFrameCount)
        {
            playing.store(false);
        }
    }

    return AUDIO_OK;
}

bool SoundPlayer::isPlaying() const
{
    return playing.load();
}
