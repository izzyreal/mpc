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

unsigned int SoundRecorder::getInputGain() const
{
    return inputGain;
}

void SoundRecorder::setInputGain(const unsigned int gain)
{
    if (gain <= 100)
    {
        inputGain = gain;
    }
}

void SoundRecorder::setArmed(const bool b)
{
    armed = b;
}
bool SoundRecorder::isArmed() const
{
    return armed;
}
bool SoundRecorder::isRecording() const
{
    return recording.load();
}

void SoundRecorder::setSampleScreenActive(const bool active)
{
    sampleScreenActive.store(active);
}

void SoundRecorder::prepare(const std::shared_ptr<Sound> &soundToUse,
                            const int newLengthInFrames,
                            const int engineSampleRateToUse)
{
    // Must not be preparing while audio thread is actively recording
    if (recording.load())
    {
        return;
    }

    sound = soundToUse;
    assert(sound->getSampleData()->empty());

    engineSampleRate = engineSampleRateToUse;
    const double rateRatio = static_cast<double>(engineSampleRate) / 44100.0;
    lengthInFramesAtEngineSampleRate =
        static_cast<int>(std::lround(newLengthInFrames * rateRatio));

    const auto sampleScreen = mpc.screens->get<ScreenId::SampleScreen>();

    // compute preRec in engine-rate frames and add it to total length
    const int preRecMs = sampleScreen->preRec;
    preRecFramesAtEngineSampleRate =
        static_cast<size_t>(std::lround(engineSampleRate * 0.001 * preRecMs));

    lengthInFramesAtEngineSampleRate +=
        static_cast<int>(preRecFramesAtEngineSampleRate);

    cancelled = false;
    mode = sampleScreen->getMode();
    if (mode != 2)
    {
        sound->setMono(true);
    }

    writeIndex.store(0, std::memory_order_relaxed);
    readIndex = 0;

    resamplers[0].reset();
    resamplers[1].reset();

    recordedFrameCountAtEngineSampleRate = 0;
}

void SoundRecorder::start()
{
    if (recording.load())
    {
        return;
    }

    armed = false;

    const size_t w = writeIndex.load(std::memory_order_acquire);
    const size_t bufSize = ringLeft.size();

    const size_t preRec = std::min(preRecFramesAtEngineSampleRate, bufSize - 1);
    const size_t start = (w + bufSize - preRec) % bufSize;
    readIndex = start;

    recordedFrameCountAtEngineSampleRate = 0;
    recording.store(true, std::memory_order_release);
}

void SoundRecorder::cancel()
{
    cancelled = true;
}

int SoundRecorder::processAudio(AudioBuffer *buf, const int nFrames)
{
    processing.store(true, std::memory_order_release);
    const auto sampleScreen = mpc.screens->get<ScreenId::SampleScreen>();

    if (!sampleScreenActive.load())
    {
        if (lastSampleScreenActive)
        {
            lastSampleScreenActive = false;
        }
        processing.store(false, std::memory_order_release);
        return AUDIO_OK;
    }

    lastSampleScreenActive = true;
    mode = sampleScreen->getMode();

    const float gain = inputGain * 0.01f;
    float peakL = 0.f, peakR = 0.f;
    const size_t bufSize = ringLeft.size();

    // Write incoming frames into the ring buffer (always)
    for (int i = 0; i < nFrames; ++i)
    {
        const float l =
            clamp_mean_normalized_float(buf->getChannel(0)[i] * gain);
        const float r =
            clamp_mean_normalized_float(buf->getChannel(1)[i] * gain);
        leftChannelCopy[i] = l;
        rightChannelCopy[i] = r;

        // atomic writeIndex write
        const size_t w = writeIndex.load(std::memory_order_relaxed);
        ringLeft[w] = l;
        ringRight[w] = r;
        const size_t wnext = (w + 1) % bufSize;
        writeIndex.store(wnext, std::memory_order_release);

        peakL = std::max(peakL, l);
        peakR = std::max(peakR, r);
    }

    sampleScreen->setCurrentBufferPeak({peakL, peakR});

    // armed trigger check
    if (armed && (log10(peakL) * 20 > sampleScreen->threshold ||
                  log10(peakR) * 20 > sampleScreen->threshold))
    {
        armed = false;
        mpc.getEngineHost()->startRecordingSound();
    }

    // If not recording, nothing to consume this frame (we keep always-writing)
    if (!recording.load(std::memory_order_acquire))
    {
        processing.store(false, std::memory_order_release);
        return AUDIO_OK;
    }

    // When recording, read up to nFrames from ring at readIndex (audio-thread
    // owned)
    int consumed = 0;
    for (int i = 0; i < nFrames; ++i)
    {
        // Safety: ensure we don't read past writer in a pathological
        // buffer-overrun case. But since audio thread is both writer and
        // reader, and buffer is sized sufficiently, this loop will normally
        // always read `nFrames`.
        unresampledLeft[i] = ringLeft[readIndex];
        unresampledRight[i] = ringRight[readIndex];
        readIndex = (readIndex + 1) % bufSize;
        consumed++;
    }

    if (engineSampleRate != 44100)
    {
        if (mode == 0 || mode == 1)
        {
            const auto &input = mode == 0 ? unresampledLeft : unresampledRight;
            const auto generatedFrameCount = resamplers[0].resample(
                input, resampledLeft, engineSampleRate, consumed);
            sound->appendFrames(resampledLeft, generatedFrameCount);
        }
        else
        {
            const auto genL = resamplers[0].resample(
                unresampledLeft, resampledLeft, engineSampleRate, consumed);
            const auto genR = resamplers[1].resample(
                unresampledRight, resampledRight, engineSampleRate, consumed);
            assert(genL == genR);
            sound->appendFrames(resampledLeft, resampledRight, genL);
        }
    }
    else
    {
        if (mode == 0)
        {
            sound->appendFrames(unresampledLeft, consumed);
        }
        else if (mode == 1)
        {
            sound->appendFrames(unresampledRight, consumed);
        }
        else
        {
            sound->appendFrames(unresampledLeft, unresampledRight, consumed);
        }
    }

    recordedFrameCountAtEngineSampleRate += consumed;
    if (recordedFrameCountAtEngineSampleRate >=
        lengthInFramesAtEngineSampleRate)
    {
        recording.store(false, std::memory_order_release);
    }
    processing.store(false, std::memory_order_release);
    return AUDIO_OK;
}

void SoundRecorder::stop()
{
    // request stop (works if caller called from UI thread)
    recording.store(false, std::memory_order_release);

    // Wait for audio-thread to finish its current processing/append.
    // Spin-wait briefly — this should be short (one audio callback).
    while (processing.load(std::memory_order_acquire))
    {
        std::this_thread::yield();
    }

    // Called on UI thread after audio-thread set recording=false.
    // If cancelled, delete sound and return.
    if (cancelled)
    {
        mpc.getSampler()->deleteSound(sound);
        cancelled = false;
        return;
    }

    if (engineSampleRate != 44100)
    {
        if (mode == 0 || mode == 1)
        {
            if (const auto remaining =
                    resamplers[0].wrapUpAndGetRemainder(resampledLeft);
                remaining > 0)
            {
                sound->appendFrames(resampledLeft, remaining);
            }
        }
        else if (mode == 2)
        {
            const auto remainingL =
                resamplers[0].wrapUpAndGetRemainder(resampledLeft);
            const auto remainingR =
                resamplers[1].wrapUpAndGetRemainder(resampledRight);
            assert(remainingL == remainingR);
            if (remainingL > 0)
            {
                sound->appendFrames(resampledLeft, resampledRight, remainingL);
            }
        }
    }

    // By design the audio thread already appended samples into `sound`.
    // Here we only do post-processing: trim overflow and set start/loop/end.
    const double invRatio = 44100.0 / static_cast<double>(engineSampleRate);
    const int lengthInFramesAt44Khz = static_cast<int>(
        std::lround(lengthInFramesAtEngineSampleRate * invRatio));

    if (const int overflowAt44Khz =
            sound->getFrameCount() - lengthInFramesAt44Khz;
        overflowAt44Khz > 0)
    {
        sound->removeFramesFromEnd(overflowAt44Khz);
    }

    const auto sampleScreen = mpc.screens->get<ScreenId::SampleScreen>();
    const int preRecFramesAt44Khz =
        static_cast<int>(44.1 * sampleScreen->preRec);
    sound->setStart(preRecFramesAt44Khz);
    sound->setLoopTo(sound->getFrameCount());
    sound->setEnd(sound->getFrameCount());

    mpc.getLayeredScreen()->openScreenById(ScreenId::KeepOrRetryScreen);
}
