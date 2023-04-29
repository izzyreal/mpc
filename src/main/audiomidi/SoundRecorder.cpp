#include <audiomidi/SoundRecorder.hpp>

#include <audiomidi/AudioMidiServices.hpp>

#include <lcdgui/screens/SampleScreen.hpp>

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

void SoundRecorder::prepare(const std::shared_ptr<Sound> soundToUse, int newLengthInFrames, int engineSampleRateToUse)
{
	if (recording)
    {
        return;
    }

    sound = soundToUse;

    assert(sound->getSampleData()->size() == 0);

    engineSampleRate = engineSampleRateToUse;
    lengthInFrames = newLengthInFrames * (engineSampleRate / 44100.f);

    cancelled = false;

    mode = mpc.screens->get<SampleScreen>("sample")->getMode();

	if (mode != 2)
    {
        sound->setMono(true);
    }

    resamplers[0].reset();
    resamplers[1].reset();
}

// Should be called from the audio thread
void SoundRecorder::start()
{
	if (recording)
    {
        return;
    }

    ringBufferLeft.reset();
    ringBufferRight.reset();

    mpc.getLayeredScreen()->getCurrentBackground()->setName("recording");

    recPointer = 0;

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

    while (!ringBufferLeft.empty())
    {
        unresampledLeft[writePointer] = ringBufferLeft.get();
        unresampledRight[writePointer] = ringBufferRight.get();
        writePointer++;
    }

    if (engineSampleRate != 44100)
    {
        if (mode == 0)
        {
            resamplers[0].resample(unresampledLeft, resampledLeft, engineSampleRate, unresampledLeft.size());
            sound->appendFrames(resampledLeft);
            auto leftRemainder = resamplers[0].wrapUpAndGetRemainder();
            sound->appendFrames(leftRemainder);
        }
        else if (mode == 1)
        {
            resamplers[1].resample(unresampledRight, resampledRight, engineSampleRate, unresampledRight.size());
            sound->appendFrames(resampledRight);
            auto rightRemainder = resamplers[1].wrapUpAndGetRemainder();
            sound->appendFrames(rightRemainder);
        }
        else if (mode == 2)
        {
            resamplers[0].resample(unresampledLeft, resampledLeft, engineSampleRate, unresampledLeft.size());
            resamplers[1].resample(unresampledRight, resampledRight, engineSampleRate, unresampledRight.size());
            sound->appendFrames(resampledLeft, resampledRight);

            auto leftRemainder = resamplers[0].wrapUpAndGetRemainder();
            auto rightRemainder = resamplers[1].wrapUpAndGetRemainder();
            sound->appendFrames(leftRemainder, rightRemainder);
        }
    }
    else
    {
        if (mode == 0)
        {
            sound->appendFrames(unresampledLeft);
        }
        else if (mode == 1)
        {
            sound->appendFrames(unresampledRight);
        }
        else if (mode == 2)
        {
            sound->appendFrames(unresampledLeft, unresampledRight);
        }
    }

    const auto sampleScreen = mpc.screens->get<SampleScreen>("sample");
    const auto preRecFramesAt44Khz = (int) (44.1 * sampleScreen->preRec);

    const int lengthInFramesAt44Khz = static_cast<int>(lengthInFrames / (engineSampleRate / 44100.f));
    const int overflowAt44Khz = sound->getFrameCount() - (lengthInFramesAt44Khz + preRecFramesAt44Khz);

    if (overflowAt44Khz > 0)
    {
        sound->removeFramesFromEnd(overflowAt44Khz);
    }

    sound->setStart(preRecFramesAt44Khz);
    sound->setEnd(sound->getFrameCount());

    mpc.getLayeredScreen()->openScreen("keep-or-retry");
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
    auto sampleScreen = mpc.screens->get<SampleScreen>("sample");

    if (!sampleScreenActive.load())
    {
        if (lastSampleScreenActive)
        {
            lastSampleScreenActive = false;
        }
        return AUDIO_OK;
    }

    lastSampleScreenActive = true;

    mode = mpc.screens->get<SampleScreen>("sample")->getMode();

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
        mpc.getLayeredScreen()->getCurrentBackground()->setName("recording");
        mpc.getAudioMidiServices()->startRecordingSound();
    }

    if (!recording)
    {
        return AUDIO_OK;
    }

    const bool shouldResample = engineSampleRate != 44100;

    if (const auto preRecFrames = (int)(engineSampleRate * 0.001 * sampleScreen->preRec);
        recPointer == 0 && preRecFrames > 0)
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
        if (mode == 0)
        {
            resamplers[0].resample(unresampledLeft, resampledLeft, engineSampleRate, nFrames);
            sound->appendFrames(resampledLeft);
        }
        else if (mode == 1)
        {
            resamplers[1].resample(unresampledRight, resampledRight, engineSampleRate, nFrames);
            sound->appendFrames(resampledRight);
        }
        else if (mode == 2)
        {
            resamplers[0].resample(unresampledLeft, resampledLeft, engineSampleRate, nFrames);
            resamplers[1].resample(unresampledRight, resampledRight, engineSampleRate, nFrames);
            sound->appendFrames(resampledLeft, resampledRight);
        }
    }
    else
    {
        if (mode == 0)
        {
            sound->appendFrames(unresampledLeft);
        }
        else if (mode == 1)
        {
            sound->appendFrames(unresampledRight);
        }
        else if (mode == 2)
        {
            sound->appendFrames(unresampledLeft, unresampledRight);
        }
    }

    recPointer += nFrames;

    if (recPointer >= lengthInFrames)
    {
        recording = false;
    }

	return AUDIO_OK;
}
