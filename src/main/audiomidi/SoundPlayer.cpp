#include "audiomidi/SoundPlayer.hpp"

#include "engine/audio/core/AudioFormat.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

#include <algorithm>

#include "SampleOps.hpp"

#include "WavInputFileStream.hpp"
#include "SndInputFileStream.hpp"

using namespace mpc::sampler;
using namespace mpc::audiomidi;

SoundPlayer::SoundPlayer()
{
    srcLeft = src_new(0, 1, &srcLeftError);
    srcRight = src_new(0, 1, &srcRightError);
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
                        SoundPlayerFileFormat fileFormatToUse,
                        int audioServerSampleRate)
{
    if (playing.load())
    {
        return false;
    }

    if (readThread.joinable())
    {
        readThread.join();
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
                                sourceNumChannels, sourceFrameCount);
    }
    else if (fileFormat == SND)
    {
        valid = snd_read_header(stream, sourceSampleRate, validBits,
                                sourceNumChannels, sourceFrameCount);
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

    while (bufferLeft.pop())
    {
    }
    while (bufferRight.pop())
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
    stopEarly = false;
    playedFrameCount = 0;
    ingestedSourceFrameCount = 0;

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
    stopEarly = true;
}

void SoundPlayer::readWithoutResampling()
{
    const auto capacity = bufferLeft.max_capacity();
    const auto channels = inputAudioFormat->getChannels();

    const auto currentBufferSpace =
        capacity -
        std::max<size_t>(bufferLeft.size_approx(), bufferRight.size_approx());

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
            currentByteIndex >= (byteCountToIngest / bytesPerSample))
        {
            break;
        }

        bufferLeft.emplace(readNextFrame());

        if (channels == 2 && fileFormat == WAV)
        {
            bufferRight.emplace(readNextFrame());
            currentByteIndex += bytesPerSample;
        }
    }

    if (channels == 2 && fileFormat == SND)
    {
        const auto bytesPerChannel = sourceFrameCount * bytesPerSample;

        stream->seekg(-(byteCountToIngest / 2) + bytesPerChannel,
                      std::ios_base::cur);

        for (int currentByteIndex = 0;
             currentByteIndex < (byteCountToIngest / 2);
             currentByteIndex += bytesPerSample)
        {
            bufferRight.emplace(readNextFrame());
        }

        stream->seekg(-bytesPerChannel, std::ios_base::cur);
    }

    ingestedSourceFrameCount += frameCountToIngest;
}

void SoundPlayer::readWithResampling(const float ratio)
{
    const auto capacity = bufferLeft.max_capacity();
    const auto channels = inputAudioFormat->getChannels();

    const auto currentBufferSpace =
        capacity -
        std::max<size_t>(bufferLeft.size_approx(), bufferRight.size_approx());

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
            currentByteIndex >= (byteCountToIngest / 2))
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

        for (int currentByteIndex = 0;
             currentByteIndex < (byteCountToIngest / 2);
             currentByteIndex += bytesPerSample)
        {
            resampleInputBufferRight[frameCounter++] = readNextFrame();
        }

        stream->seekg(-bytesPerChannel, std::ios_base::cur);
    }

    ingestedSourceFrameCount += unresampledFrameCountToIngest;

    const int endOfInput =
        (ingestedSourceFrameCount >= sourceFrameCount) ? 1 : 0;

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
        bufferLeft.enqueue(resampleOutputBuffer[f]);
    }

    if (channels == 2)
    {
        data.data_in = &resampleInputBufferRight[0];
        src_process(srcRight, &data);

        for (int f = 0; f < data.output_frames_gen; f++)
        {
            bufferRight.enqueue(resampleOutputBuffer[f]);
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
    if (inputAudioFormat->getSampleSizeInBits() == 24)
    {
        return sampleops::int24_to_float(readNext24BitInt());
    }

    return sampleops::short_to_float(readNextShort());
}

int32_t SoundPlayer::readNext24BitInt()
{
    char buffer[3];
    stream->read(buffer, 3);

    if (stream->gcount() != 3)
    {
        return 0;
    }

    int32_t value = static_cast<unsigned char>(buffer[0]) |
                    (static_cast<unsigned char>(buffer[1]) << 8) |
                    (static_cast<unsigned char>(buffer[2]) << 16);

    if (value & 0x00800000)
    {
        value |= static_cast<int32_t>(0xFF000000);
    }

    return value;
}

short SoundPlayer::readNextShort()
{
    char buffer[2];
    stream->read(buffer, 2);

    if (stream->gcount() != 2)
    {
        return 0;
    }

    const short value = static_cast<unsigned char>(buffer[0]) |
                        (static_cast<unsigned char>(buffer[1]) << 8);

    return value;
}

int SoundPlayer::processAudio(AudioBuffer *buf, int nFrames)
{
    if (!playing.load())
    {
        buf->makeSilence();
        return AUDIO_SILENCE;
    }

    size_t availableFrameCount = bufferLeft.size_approx();

    if (inputAudioFormat->getChannels() == 2)
    {
        availableFrameCount =
            std::min<size_t>(availableFrameCount, bufferRight.size_approx());
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
        outputBufferLeft[frame] = *bufferLeft.peek();
        bufferLeft.pop();

        if (inputAudioFormat->getChannels() == 2)
        {
            outputBufferRight[frame] = *bufferRight.peek();
            bufferRight.pop();
        }
        else
        {
            outputBufferRight[frame] = outputBufferLeft[frame];
        }

        if (stopEarly)
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

    const auto ratio = buf->getSampleRate() / inputAudioFormat->getSampleRate();

    if (ratio == 1.f)
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

bool SoundPlayer::isPlaying()
{
    return playing.load();
}
