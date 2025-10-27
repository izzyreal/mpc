#include "engine/audio/mixer/AudioMixerStrip.hpp"

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/audio/core/AudioControls.hpp"
#include "engine/audio/core/AudioControlsChain.hpp"
#include "engine/audio/core/AudioProcess.hpp"

#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/AudioMixerBus.hpp"
#include "engine/audio/mixer/MainMixProcess.hpp"
#include "engine/audio/mixer/MixControls.hpp"
#include "engine/audio/mixer/MixerControlsIds.hpp"

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;
using namespace std;

AudioMixerStrip::AudioMixerStrip(
    AudioMixer *mixer, std::shared_ptr<AudioControlsChain> controlsChain)
    : AudioProcessChain(controlsChain)
{
    silenceCountdown = silenceCount;
    this->mixer = mixer;
    buffer = createBuffer();
}

void AudioMixerStrip::setInputProcess(std::shared_ptr<AudioProcess> input)
{

    if (controlChain->getId() != MixerControlsIds::CHANNEL_STRIP)
    {
        return;
    }
    auto oldInput = this->input;
    if (input)
    {
        input->open();
    }

    this->input = input;
    if (oldInput)
    {
        oldInput->close();
    }
}

void AudioMixerStrip::setDirectOutputProcess(
    std::shared_ptr<AudioProcess> output)
{
    auto oldOutput = directOutput;
    if (output)
    {
        output->open();
    }

    this->directOutput = output;
    if (oldOutput)
    {
        oldOutput->close();
    }
}

void AudioMixerStrip::silence()
{
    if (nmixed > 0)
    {
        buffer->makeSilence();
        nmixed = 0;
    }
}

AudioBuffer *AudioMixerStrip::createBuffer()
{
    auto id = controlChain->getId();
    if (id == MixerControlsIds::CHANNEL_STRIP)
    {
        isChannel = true;
        return mixer->getSharedBuffer();
    }
    else if (id == MixerControlsIds::MAIN_STRIP)
    {
        return mixer->getMainBus()->getBuffer();
    }
    return mixer->getBus(getName())->getBuffer();
}

const int AudioMixerStrip::silenceCount;

bool AudioMixerStrip::processBuffer(int nFrames)
{
    auto ret = AUDIO_OK;
    if (isChannel)
    {
        if (input)
        {
            ret = input->processAudio(buffer, nFrames);

            if (ret == AUDIO_DISCONNECT)
            {
                return false;
            }
            else if (ret == AUDIO_SILENCE && silenceCountdown == 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    processAudio(buffer, nFrames);

    if (isChannel)
    {
        if (ret == AUDIO_SILENCE)
        {
            if (buffer->square() > 0.00000001f)
            {
                silenceCountdown = silenceCount;
            }
            else
            {
                silenceCountdown--;
            }
        }
        else
        {
            silenceCountdown = silenceCount;
        }
    }
    if (directOutput)
    {
        directOutput->processAudio(buffer, nFrames);
    }
    return true;
}

std::shared_ptr<AudioProcess>
AudioMixerStrip::createProcess(std::shared_ptr<AudioControls> controls)
{
    auto mixControls = dynamic_pointer_cast<MixControls>(controls);

    if (mixControls)
    {
        std::shared_ptr<AudioMixerStrip> routedStrip;
        if (mixControls->getName() == mixer->getMainBus()->getName())
        {
            routedStrip = mixer->getMainStrip();
            return std::make_shared<MainMixProcess>(routedStrip, mixControls,
                                                    mixer);
        }
        else
        {
            routedStrip = mixer->getStripImpl(mixControls->getName());
            return std::make_shared<MixProcess>(routedStrip, mixControls);
        }
    }

    return AudioProcessChain::createProcess(controls);
}

int AudioMixerStrip::mix(mpc::engine::audio::core::AudioBuffer *bufferToMix,
                         vector<float> &gain)
{
    auto doMix = buffer != bufferToMix;
    auto snc = bufferToMix->getChannelCount();
    auto dnc = buffer->getChannelCount();

    auto ns = buffer->getSampleCount();
    float g;
    auto k = static_cast<float>((snc)) / dnc;
    for (auto i = 0; i < dnc; i++)
    {
        g = gain[i] * k;
        auto &in = bufferToMix->getChannel(i % snc);
        auto &out = buffer->getChannel(i);
        if (doMix)
        {
            for (auto s = 0; s < ns; s++)
            {
                out[s] += in[s] * g;
            }
        }
        else
        {
            for (auto s = 0; s < ns; s++)
            {
                out[s] = in[s] * g;
            }
        }
    }
    auto ret = 1;

    if (!doMix)
    {
        ret |= 2;
    }

    if (ret != 0)
    {
        nmixed += 1;
    }
    return ret;
}

void AudioMixerStrip::close()
{
    AudioProcessChain::close();
    mixer->removeBuffer(buffer);
}
