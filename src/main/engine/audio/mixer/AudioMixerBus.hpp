#pragma once

#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioProcess.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>

namespace mpc::engine::audio::mixer
{
    class AudioMixer;

    class AudioMixerBus
    {

    private:
        mpc::engine::audio::core::AudioBuffer *buffer{nullptr};
        std::shared_ptr<mpc::engine::audio::core::AudioProcess> output;
        std::string name;

    public:
        mpc::engine::audio::core::AudioBuffer *getBuffer();

        std::string getName();

        void silence();

        void write(int nFrames);

    public:
        AudioMixerBus(AudioMixer *mixer,
                      std::shared_ptr<BusControls> busControls);
    };
} // namespace mpc::engine::audio::mixer
