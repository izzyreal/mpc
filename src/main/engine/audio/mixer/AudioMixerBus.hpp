#pragma once

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/audio/core/AudioProcess.hpp"
#include "engine/audio/mixer/BusControls.hpp"

namespace mpc::engine::audio::mixer
{
    class AudioMixer;

    class AudioMixerBus
    {

    private:
        std::shared_ptr<audio::core::AudioBuffer> buffer;
        std::shared_ptr<core::AudioProcess> output;
        std::string name;

    public:
        std::shared_ptr<core::AudioBuffer> getBuffer() const;

        std::string getName();

        void silence() const;

        void write(int nFrames) const;

    public:
        AudioMixerBus(AudioMixer *mixer,
                      const std::shared_ptr<BusControls> &busControls);
    };
} // namespace mpc::engine::audio::mixer
