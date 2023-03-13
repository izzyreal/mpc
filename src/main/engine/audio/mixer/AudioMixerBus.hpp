#pragma once

#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioProcess.hpp>
#include <engine/audio/core/ChannelFormat.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>

namespace ctoot::audio::mixer {
    class AudioMixer;

    class AudioMixerBus
    {

    private:
        AudioMixer *mixer{nullptr};
        ctoot::audio::core::AudioBuffer *buffer{nullptr};
        std::shared_ptr<ctoot::audio::core::AudioProcess> output;
        ctoot::audio::core::AudioProcess *meter{nullptr};
        std::string name;
        std::shared_ptr<ctoot::audio::core::ChannelFormat> channelFormat;

    public:
        virtual ctoot::audio::core::AudioBuffer *getBuffer();

        virtual std::string getName();

        virtual void silence();

        virtual void write(int nFrames);

        virtual void close();

    public:
        AudioMixerBus(AudioMixer *mixer, std::shared_ptr<BusControls> busControls);

        virtual ~AudioMixerBus();
    };
}
