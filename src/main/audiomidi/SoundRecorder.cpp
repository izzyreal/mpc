#include "audiomidi/SoundRecorder.hpp"

#include "audiomidi/AudioMidiServices.hpp"

#include "lcdgui/screens/SampleScreen.hpp"

#include <SampleOps.hpp>

#include <cmath>

using namespace mpc::sampler;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::audio::core;
using namespace mpc::sampleops;

SoundRecorder::SoundRecorder(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

unsigned int SoundRecorder::getInputGain()
{
	return inputGain;
}

void SoundRecorder::setInputGain(unsigned int gain)
{
	if (gain > 100)
    {
        return;
    }

	inputGain = gain;
}

void SoundRecorder::setArmed(bool b)
{
	armed = b;
}

bool SoundRecorder::isArmed()
{
	return armed;
}

void SoundRecorder::prepare(const std::shared_ptr<Sound>& soundToUse, int newLengthInFrames, int engineSampleRateToUse)
{
	  if (recording)
    {
        return;
    }

    sound = soundToUse;

    assert(sound->getSampleData()->empty());

    engineSampleRate = engineSampleRateToUse;
    lengthInFramesAtEngineSampleRate = newLengthInFrames * (engineSampleRate / 44100.f);

    const auto sampleScreen = mpc.screens->get<SampleScreen>();
    const auto preRecFramesAt44Khz = (int) (44.1 * sampleScreen->preRec);
    
    lengthInFramesAtEngineSampleRate += preRecFramesAt44Khz * (engineSampleRate / 44100.f);

    cancelled = false;

    mode = mpc.screens->get<SampleScreen>()->getMode();

	  if (mode != 2)
    {
        sound->setMono(true);
    }

    ringBufferLeft.reset();
    ringBufferRight.reset();

    resamplers[0].reset();
    resamplers[1].reset();

    recordedFrameCountAtEngineSampleRate = 0;
}

// Should be called from the audio thread
void SoundRecorder::start()
{
	if (recording)
    {
        return;
    }

    mpc.getLayeredScreen()->getCurrentBackground()->setBackgroundName("recording");

    armed = false;
    recording = true;
}

bool SoundRecorder::isRecording()
{
	return recording;
}

void SoundRecorder::stop()
{
    recording = false;

    if (cancelled)
    {
        mpc.getSampler()->deleteSound(sound);
        cancelled = false;
        return;
    }

    int writePointer = 0;

    const int ringBufferRemainingFrameCount = ringBufferLeft.available();

    while (!ringBufferLeft.empty())
    {
        unresampledLeft[writePointer] = ringBufferLeft.get();
        unresampledRight[writePointer] = ringBufferRight.get();
        writePointer++;
    }

    if (engineSampleRate != 44100)
    {
        if (mode == 0 || mode == 1)
        {
            const auto& input = mode == 0 ? unresampledLeft : unresampledRight;
            const auto generatedFrameCount = resamplers[0].resample(
                    input, resampledLeft, engineSampleRate, ringBufferRemainingFrameCount);

            sound->appendFrames(resampledLeft, generatedFrameCount);

            const auto remainingFrameCount = resamplers[0].wrapUpAndGetRemainder(resampledLeft);
            sound->appendFrames(resampledLeft, remainingFrameCount);
        }
        else if (mode == 2)
        {
            const auto generatedFrameCountL = resamplers[0].resample(
                    unresampledLeft, resampledLeft, engineSampleRate, ringBufferRemainingFrameCount);

            const auto generatedFrameCountR = resamplers[1].resample(
                    unresampledRight, resampledRight, engineSampleRate, ringBufferRemainingFrameCount);

            assert(generatedFrameCountL == generatedFrameCountR);

            sound->appendFrames(resampledLeft, resampledRight, generatedFrameCountL);

            const auto remainingFrameCountL = resamplers[0].wrapUpAndGetRemainder(resampledLeft);

            const auto remainingFrameCountR = resamplers[1].wrapUpAndGetRemainder(resampledRight);

            assert(remainingFrameCountL == remainingFrameCountR);

            sound->appendFrames(resampledLeft, resampledRight, remainingFrameCountL);
        }
    }
    else
    {
        if (mode == 0)
        {
            sound->appendFrames(unresampledLeft, ringBufferRemainingFrameCount);
        }
        else if (mode == 1)
        {
            sound->appendFrames(unresampledRight, ringBufferRemainingFrameCount);
        }
        else if (mode == 2)
        {
            sound->appendFrames(unresampledLeft, unresampledRight, ringBufferRemainingFrameCount);
        }
    }

    const int lengthInFramesAt44Khz = static_cast<int>(lengthInFramesAtEngineSampleRate / (engineSampleRate / 44100.f));
    const int overflowAt44Khz = sound->getFrameCount() - lengthInFramesAt44Khz;

    if (overflowAt44Khz > 0)
    {
        sound->removeFramesFromEnd(overflowAt44Khz);
    }

    const auto sampleScreen = mpc.screens->get<SampleScreen>();
    const auto preRecFramesAt44Khz = (int) (44.1 * sampleScreen->preRec);

    sound->setStart(preRecFramesAt44Khz);
    sound->setLoopTo(sound->getFrameCount());
    sound->setEnd(sound->getFrameCount());

    mpc.getLayeredScreen()->openScreen<KeepOrRetryScreen>();
}

void SoundRecorder::cancel()
{
	cancelled = true;
}

void SoundRecorder::setSampleScreenActive(bool active)
{
	sampleScreenActive.store(active);
}

int SoundRecorder::processAudio(AudioBuffer* buf, int nFrames)
{
    auto sampleScreen = mpc.screens->get<SampleScreen>();

    if (!sampleScreenActive.load())
    {
        if (lastSampleScreenActive)
        {
            lastSampleScreenActive = false;
        }
        return AUDIO_OK;
    }

    lastSampleScreenActive = true;

    mode = mpc.screens->get<SampleScreen>()->getMode();

    const auto gain = inputGain * 0.01;
    float peakL = 0, peakR = 0;

    for (int i = 0; i < nFrames; i++)
    {
        leftChannelCopy[i] = clamp_mean_normalized_float(buf->getChannel(0)[i] * gain);
        rightChannelCopy[i] = clamp_mean_normalized_float(buf->getChannel(1)[i] * gain);

        ringBufferLeft.put(leftChannelCopy[i]);
        ringBufferRight.put(rightChannelCopy[i]);

        peakL = std::max<float>(peakL, leftChannelCopy[i]);
        peakR = std::max<float>(peakR, rightChannelCopy[i]);
    }

    notifyObservers(std::pair<float, float>(peakL, peakR));

    // Is this comparison correct or does the real 2KXL take Mode into account?
    // Also, does the real 2KXL do the below in a frame-accurate manner?
    if (armed && (log10(peakL) * 20 > sampleScreen->threshold || log10(peakR) * 20 > sampleScreen->threshold))
    {
        armed = false;
        mpc.getLayeredScreen()->getCurrentBackground()->setBackgroundName("recording");
        mpc.getAudioMidiServices()->startRecordingSound();
    }

    if (!recording)
    {
        return AUDIO_OK;
    }

    const bool shouldResample = engineSampleRate != 44100;

    if (const auto preRecFrames = (int)(engineSampleRate * 0.001 * sampleScreen->preRec);
        recordedFrameCountAtEngineSampleRate == 0 && preRecFrames > 0)
    {
        const int numFramesToMoveBack = preRecFrames;
        ringBufferLeft.moveTailToHead();
        ringBufferLeft.move(-numFramesToMoveBack);
        ringBufferRight.moveTailToHead();
        ringBufferRight.move(-numFramesToMoveBack);
    }

    for (int i = 0; i < nFrames; i++)
    {
        unresampledLeft[i] = ringBufferLeft.get();
        unresampledRight[i] = ringBufferRight.get();
    }

    if (shouldResample)
    {
        if (mode == 0 || mode == 1)
        {
            const auto& input = mode == 0 ? unresampledLeft : unresampledRight;
            const auto generatedFrameCount = resamplers[0].resample(input, resampledLeft, engineSampleRate, nFrames);
            sound->appendFrames(resampledLeft, generatedFrameCount);
        }
        else if (mode == 2)
        {
            const auto generatedFrameCountL = resamplers[0].resample(unresampledLeft, resampledLeft, engineSampleRate, nFrames);
            const auto generatedFrameCountR = resamplers[1].resample(unresampledRight, resampledRight, engineSampleRate, nFrames);

            assert(generatedFrameCountL == generatedFrameCountR);

            sound->appendFrames(resampledLeft, resampledRight, generatedFrameCountL);
        }
    }
    else
    {
        if (mode == 0)
        {
            sound->appendFrames(unresampledLeft, nFrames);
        }
        else if (mode == 1)
        {
            sound->appendFrames(unresampledRight, nFrames);
        }
        else if (mode == 2)
        {
            sound->appendFrames(unresampledLeft, unresampledRight, nFrames);
        }
    }

    recordedFrameCountAtEngineSampleRate += nFrames;

    if (recordedFrameCountAtEngineSampleRate >= lengthInFramesAtEngineSampleRate)
    {
        recording = false;
    }

	return AUDIO_OK;
}
