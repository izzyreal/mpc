#include "audiomidi/SoundRecorder.hpp"

#include "Mpc.hpp"
#include "engine/EngineHost.hpp"

#include "lcdgui/screens/SampleScreen.hpp"
#include "sampler/Sampler.hpp"

#include "SampleOps.hpp"

#include <algorithm>
#include <cassert>

using namespace mpc::sampler;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::audio::core;
using namespace mpc::sampleops;

SoundRecorder::SoundRecorder(Mpc &mpc)
    : mpc(mpc), leftChannelCopy(INTERNAL_BUF_SIZE),
      rightChannelCopy(INTERNAL_BUF_SIZE), unresampledLeft(INTERNAL_BUF_SIZE),
      unresampledRight(INTERNAL_BUF_SIZE), resampledLeft(INTERNAL_BUF_SIZE),
      resampledRight(INTERNAL_BUF_SIZE), ringLeft(INTERNAL_BUF_SIZE),
      ringRight(INTERNAL_BUF_SIZE)
{
}

int8_t SoundRecorder::getInputGain() const
{
    return inputGain.load(std::memory_order_relaxed);
}

void SoundRecorder::setInputGain(const int8_t i)
{
    inputGain.store(std::clamp(i, int8_t{0}, int8_t{100}),
                    std::memory_order_relaxed);
}

void SoundRecorder::setArmed(const bool b)
{
    armed.store(b, std::memory_order_relaxed);
}

bool SoundRecorder::isArmed() const
{
    return armed.load(std::memory_order_relaxed);
}

bool SoundRecorder::isRecording() const
{
    return recording.load(std::memory_order_relaxed);
}

void SoundRecorder::setSampleScreenActive(const bool active)
{
    sampleScreenActive.store(active, std::memory_order_relaxed);
}

void SoundRecorder::prepare(const std::shared_ptr<Sound> &soundToUse,
                            const int newLengthInFrames,
                            const int engineSampleRateToUse)
{
    if (recording.load(std::memory_order_acquire))
    {
        return;
    }

    sound = soundToUse;
    assert(sound->getSampleData()->empty());

    engineSampleRate = engineSampleRateToUse;

    const auto sampleScreen = mpc.screens->get<ScreenId::SampleScreen>();
    const int preRecMs = sampleScreen->preRec;

    const int preRecFramesAt44k =
        static_cast<int>(std::lround(44.1 * preRecMs));

    const int totalFramesAt44k =
        newLengthInFrames + preRecFramesAt44k;

    lengthInFramesAtEngineSampleRate = totalFramesAt44k;

    preRecFramesAtEngineSampleRate =
        static_cast<size_t>(std::lround(engineSampleRate * 0.001 * preRecMs));

    cancelled.store(false, std::memory_order_relaxed);

    mode = sampleScreen->getMode();

    if (mode == 2)
    {
        sound->setMono(false);
        sound->getMutableSampleData()->reserve(totalFramesAt44k * 2);
    }
    else
    {
        sound->setMono(true);
        sound->getMutableSampleData()->reserve(totalFramesAt44k);
    }

    writeIndex = 0;
    readIndex = 0;

    resamplers[0].reset();
    resamplers[1].reset();

    recordedFrameCountAtEngineSampleRate = 0;
}

void SoundRecorder::start()
{
    if (recording.load(std::memory_order_relaxed))
    {
        return;
    }

    armed.store(false, std::memory_order_release);

    const size_t w = writeIndex;
    const size_t bufSize = ringLeft.size();

    const size_t preRec = std::min(preRecFramesAtEngineSampleRate, bufSize - 1);
    const size_t start = (w + bufSize - preRec) % bufSize;
    readIndex = start;

    recordedFrameCountAtEngineSampleRate = 0;
    recording.store(true, std::memory_order_relaxed);
}

void SoundRecorder::cancel()
{
    cancelled.store(true, std::memory_order_relaxed);
}

int SoundRecorder::processAudio(AudioBuffer *buf, const int nFrames)
{
    const auto sampleScreen = mpc.screens->get<ScreenId::SampleScreen>();

    if (!sampleScreenActive.load(std::memory_order_relaxed))
    {
        return AUDIO_OK;
    }

    mode = sampleScreen->getMode();

    const float gain = inputGain * 0.01f;
    float peakL = 0.f, peakR = 0.f;
    const size_t bufSize = ringLeft.size();

    for (int i = 0; i < nFrames; ++i)
    {
        const float l =
            clamp_mean_normalized_float(buf->getChannel(0)[i] * gain);
        const float r =
            clamp_mean_normalized_float(buf->getChannel(1)[i] * gain);
        leftChannelCopy[i] = l;
        rightChannelCopy[i] = r;

        const size_t w = writeIndex;
        ringLeft[w] = l;
        ringRight[w] = r;
        writeIndex = (w + 1) % bufSize;

        peakL = std::max(peakL, l);
        peakR = std::max(peakR, r);
    }

    sampleScreen->setCurrentBufferPeak({peakL, peakR});

    const int threshold =
        sampleScreen->threshold.load(std::memory_order_relaxed);

    if (armed.load(std::memory_order_relaxed) &&
        (log10(peakL) * 20 > static_cast<float>(threshold) ||
         log10(peakR) * 20 > static_cast<float>(threshold)))
    {
        armed.store(false, std::memory_order_relaxed);
        mpc.getEngineHost()->startRecordingSound();
    }

    if (!recording.load(std::memory_order_relaxed))
    {
        return AUDIO_OK;
    }

    int consumed = 0;
    for (int i = 0; i < nFrames; ++i)
    {
        unresampledLeft[i] = ringLeft[readIndex];
        unresampledRight[i] = ringRight[readIndex];
        readIndex = (readIndex + 1) % bufSize;
        consumed++;
    }

    const int remaining =
        lengthInFramesAtEngineSampleRate - recordedFrameCountAtEngineSampleRate;

    if (remaining <= 0)
    {
        recording.store(false, std::memory_order_relaxed);
        return AUDIO_OK;
    }

    if (engineSampleRate != 44100)
    {
        if (mode == 0 || mode == 1)
        {
            const auto &input = mode == 0 ? unresampledLeft : unresampledRight;

            const int gen = resamplers[0].resample(input, resampledLeft,
                                                   engineSampleRate, consumed);

            const int toAppend = std::min(gen, remaining);
            if (toAppend > 0)
            {
                sound->appendFrames(resampledLeft, toAppend);
                recordedFrameCountAtEngineSampleRate += toAppend;
            }
            if (toAppend < gen)
            {
                recording.store(false, std::memory_order_relaxed);
            }
        }
        else
        {
            const int genL = resamplers[0].resample(
                unresampledLeft, resampledLeft, engineSampleRate, consumed);
            const int genR = resamplers[1].resample(
                unresampledRight, resampledRight, engineSampleRate, consumed);
            assert(genL == genR);
            const int toAppend = std::min(genL, remaining);
            if (toAppend > 0)
            {
                sound->appendFrames(resampledLeft, resampledRight, toAppend);
                recordedFrameCountAtEngineSampleRate += toAppend;
            }
            if (toAppend < genL)
            {
                recording.store(false, std::memory_order_relaxed);
            }
        }
    }
    else
    {
        const int toAppend = std::min(consumed, remaining);
        if (toAppend > 0)
        {
            if (mode == 0)
            {
                sound->appendFrames(unresampledLeft, toAppend);
            }
            else if (mode == 1)
            {
                sound->appendFrames(unresampledRight, toAppend);
            }
            else
            {
                sound->appendFrames(unresampledLeft, unresampledRight,
                                    toAppend);
            }
            recordedFrameCountAtEngineSampleRate += toAppend;
        }
        if (toAppend < consumed)
        {
            recording.store(false, std::memory_order_relaxed);
        }
    }

    return AUDIO_OK;
}

void SoundRecorder::stop()
{
    recording.store(false, std::memory_order_release);

    if (cancelled.load(std::memory_order_relaxed))
    {
        mpc.getSampler()->deleteSound(sound);
        cancelled.store(false, std::memory_order_relaxed);
        return;
    }

    if (engineSampleRate != 44100)
    {
        if (mode == 0 || mode == 1)
        {
            const int remaining = lengthInFramesAtEngineSampleRate -
                                  recordedFrameCountAtEngineSampleRate;

            if (remaining > 0)
            {
                const auto gen =
                    resamplers[0].wrapUpAndGetRemainder(resampledLeft);

                if (const auto toAppend =
                        std::min(static_cast<int>(gen), remaining);
                    toAppend > 0)
                {
                    sound->appendFrames(resampledLeft, toAppend);
                }
            }
        }
        else if (mode == 2)
        {
            const auto remaining = lengthInFramesAtEngineSampleRate -
                                   recordedFrameCountAtEngineSampleRate;

            if (remaining > 0)
            {
                const auto genL =
                    resamplers[0].wrapUpAndGetRemainder(resampledLeft);

                const auto genR =
                    resamplers[1].wrapUpAndGetRemainder(resampledRight);

                assert(genL == genR);

                if (const auto toAppend =
                        std::min(static_cast<int>(genL), remaining);
                    toAppend > 0)
                {
                    sound->appendFrames(resampledLeft, resampledRight,
                                        toAppend);
                }
            }
        }
    }

    const auto sampleScreen = mpc.screens->get<ScreenId::SampleScreen>();
    const int preRecFramesAt44Khz =
        static_cast<int>(44.1 * sampleScreen->preRec);
    sound->setStart(preRecFramesAt44Khz);
    sound->setLoopTo(sound->getFrameCount());
    sound->setEnd(sound->getFrameCount());

    auto ls = mpc.getLayeredScreen();
    ls->postToUiThread(utils::Task(
        [ls, this]
        {
            sound->getMutableSampleData()->shrink_to_fit();
            sound.reset();
            ls->openScreenById(ScreenId::KeepOrRetryScreen);
        }));
}
