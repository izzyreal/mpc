#include <audiomidi/SoundPlayer.hpp>

#include <engine/audio/core/AudioFormat.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <cmath>
#include <algorithm>


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
    delete srcLeft;
    delete srcRight;
}

bool SoundPlayer::start(std::shared_ptr<std::istream> _istream, SoundPlayerFileFormat f) {
    std::unique_lock<std::mutex> guard(_playing);

    if (playing) return false;

    fileFormat = f;

    int validBits;
    int sourceSampleRate;
    int sourceNumChannels;

    bool valid = false;

    if (fileFormat == WAV)
    {
        valid = wav_read_header(_istream, sourceSampleRate, validBits, sourceNumChannels, sourceFrameCount);
    }
    else if (fileFormat == SND) {
        valid = snd_read_header(_istream, sourceSampleRate, validBits, sourceNumChannels, sourceFrameCount);
    }

    if (!valid) {
        return false;
    }

    audioFormat = std::make_shared<AudioFormat>(sourceSampleRate, validBits, sourceNumChannels, true, false);

    src_reset(srcLeft);
    src_reset(srcRight);

    fadeFactor = -1.0f;
    stopEarly = false;

    stream = _istream;

    playing = true;

    return true;
}

void SoundPlayer::enableStopEarly()
{

    std::unique_lock<std::mutex> guard(_playing);

    stopEarly = true;
    fadeFactor = 1.0f;
}

void SoundPlayer::stop()
{
    std::unique_lock<std::mutex> guard(_playing);

    if (!playing) {
        return;
    }

    playing = false;

    stream.reset();

    ingestedSourceFrameCount = 0;

    resampleInputBufferLeft.reset();
    resampleInputBufferRight.reset();
    resampleOutputBufferLeft.reset();
    resampleOutputBufferRight.reset();
}

int SoundPlayer::processAudio(AudioBuffer* buf, int nFrames)
{
    std::unique_lock<std::mutex> guard(_playing);

    auto& left = buf->getChannel(0);
    auto& right = buf->getChannel(1);

    if (!playing) {
        buf->makeSilence();
        return AUDIO_SILENCE;
    }

    auto resampleRatio = static_cast<float>(audioFormat->getSampleRate()) / static_cast<float>(buf->getSampleRate());

    auto frameCountToRead = (int) ceil(resampleRatio * nFrames);

    auto shouldStop = false;

    if (ingestedSourceFrameCount + frameCountToRead >= sourceFrameCount) {
        shouldStop = true;
        frameCountToRead = sourceFrameCount - ingestedSourceFrameCount;
    }

    auto sourceBuffer = std::make_shared<AudioBuffer>("temp", audioFormat->getChannels(), frameCountToRead, audioFormat->getSampleRate());
    std::vector<char> byteBuffer(sourceBuffer->getByteArrayBufferSize(audioFormat.get()));

    if (fileFormat == WAV || (fileFormat == SND && audioFormat->getChannels() == 1)) {
        stream->read(&byteBuffer[0], byteBuffer.size());
    }
    else {
        auto bytesPerChannel = (int) (byteBuffer.size() * 0.5);

        std::vector<char> byteBufferLeft(bytesPerChannel);
        std::vector<char> byteBufferRight(bytesPerChannel);

        auto totalChannelLengthInBytes = sourceFrameCount * (audioFormat->getSampleSizeInBits() / 8);

        stream->read(&byteBufferLeft[0], bytesPerChannel);
        stream->seekg(-bytesPerChannel + totalChannelLengthInBytes, std::ios_base::cur);

        stream->read(&byteBufferRight[0], bytesPerChannel);

        stream->seekg(-totalChannelLengthInBytes, std::ios_base::cur);

        int byteCounter = 0;

        for (int i = 0; i < byteBuffer.size(); i += 4) {
            byteBuffer[i] = byteBufferLeft[byteCounter];
            byteBuffer[i + 1] = byteBufferLeft[byteCounter + 1];
            byteBuffer[i + 2] = byteBufferRight[byteCounter];
            byteBuffer[i + 3] = byteBufferRight[byteCounter + 1];
            byteCounter += 2;
        }
    }

    sourceBuffer->initFromByteArray_(byteBuffer, 0, byteBuffer.size(), audioFormat.get());

    if (shouldStop) {
        buf->makeSilence();
    }

    const bool resample = buf->getSampleRate() != audioFormat->getSampleRate();

    if (resample) {

        if (audioFormat->getChannels() >= 1) {
            resampleChannel(true, sourceBuffer->getChannel(0), audioFormat->getSampleRate(), buf->getSampleRate());
        }

        if (audioFormat->getChannels() >= 2) {
            resampleChannel(false, sourceBuffer->getChannel(1), audioFormat->getSampleRate(), buf->getSampleRate());
        }

        if (resampleOutputBufferLeft.available() >= nFrames) {

            for (int i = 0; i < nFrames; i++) {
                left[i] = resampleOutputBufferLeft.get();
            }

            if (audioFormat->getChannels() == 1) {
                buf->copyChannel(0, 1);
            }
            else {
                for (int i = 0; i < nFrames; i++) {
                    right[i] = resampleOutputBufferRight.get();
                }
            }
        }
        else if (shouldStop && resampleOutputBufferLeft.available() > 0) {

            auto remaining = resampleOutputBufferLeft.available();

            for (int i = 0; i < std::min( (int) remaining, nFrames); i++) {
                left[i] = resampleOutputBufferLeft.get();
            }

            if (audioFormat->getChannels() == 1) {
                buf->copyChannel(0, 1);
            }
            else {
                for (int i = 0; i < std::min( (int)remaining, nFrames); i++) {
                    right[i] = resampleOutputBufferRight.get();
                }
            }
        }
    }
    else {

        auto frameCountToWrite = std::min(sourceBuffer->getSampleCount(), nFrames);
        if (sourceBuffer->getChannelCount() == 1) {

            for (int i = 0; i < frameCountToWrite; i++) {
                left[i] = sourceBuffer->getChannel(0)[i];
                right[i] = left[i];
            }
        }
        else {
            for (int i = 0; i < frameCountToWrite; i++) {
                left[i] = sourceBuffer->getChannel(0)[i];
                right[i] = sourceBuffer->getChannel(1)[i];
            }
        }
    }

    ingestedSourceFrameCount += frameCountToRead;

    if (stopEarly) {
        int bufferIndex = 0;

        while (fadeFactor >= 0.0f && bufferIndex < nFrames) {
            left[bufferIndex] = left[bufferIndex] * fadeFactor;
            right[bufferIndex] = right[bufferIndex] * fadeFactor;
            fadeFactor -= 0.002f;
            bufferIndex++;
        }

        if (bufferIndex != nFrames) {
            for (int i = bufferIndex; i < nFrames; i++) {
                left[i] = 0;
                right[i] = 0;
            }
        }
    }


    if (shouldStop || (stopEarly && fadeFactor < 0) ) {
        guard.unlock();
        stop();
    }
    else {
        guard.unlock();
    }

    return AUDIO_OK;
}

void SoundPlayer::resampleChannel(bool left, std::vector<float> &inputBuffer, int sourceSampleRate,
                                  int destinationSampleRate)
{
    auto ratio = static_cast<float>(destinationSampleRate) / static_cast<float>(sourceSampleRate);
    auto circularInputBuffer = left ? &resampleInputBufferLeft : &resampleInputBufferRight;
    auto circularOutputBuffer = left ? &resampleOutputBufferLeft : &resampleOutputBufferRight;

    for (auto f : inputBuffer) {
        circularInputBuffer->put(f);
    }

    std::vector<float> input;
    while (!circularInputBuffer->empty()) {
        input.push_back(circularInputBuffer->get());
    }

    auto output = std::vector<float>(ceil(input.size() * ratio));

    SRC_DATA data;
    data.data_in = &input[0];
    data.input_frames = input.size();
    data.data_out = &output[0];
    data.output_frames = output.size();
    data.end_of_input = 0;
    data.src_ratio = ratio;

    src_process(left ? srcLeft : srcRight, &data);

    circularInputBuffer->move(-(input.size() - data.input_frames_used));

    for (int i = 0; i < data.output_frames_gen; i++) {
        circularOutputBuffer->put(output[i]);
    }
}

bool SoundPlayer::isPlaying()
{
    return playing;
}
