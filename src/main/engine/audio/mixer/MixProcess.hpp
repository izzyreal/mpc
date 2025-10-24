#pragma once

#include <engine/audio/core/AudioProcess.hpp>
#include <engine/audio/mixer/MixControls.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>

namespace mpc::engine::audio::mixer
{

    class MixProcess : public mpc::engine::audio::core::AudioProcess
    {

    protected:
        std::shared_ptr<MixControls> mixControls;
        std::shared_ptr<AudioMixerStrip> routedStrip;

    private:
        float gain{1.0f};
        std::vector<float> channelGains;
        std::vector<float> smoothedChannelGains;
        float smoothingFactor{0};

    public:
        virtual AudioMixerStrip *getRoutedStrip();

    public:
        int processAudio(mpc::engine::audio::core::AudioBuffer *buffer) override;

    public:
        MixProcess(const std::shared_ptr<AudioMixerStrip> &strip, const std::shared_ptr<MixControls> &mixControls);
    };

} // namespace mpc::engine::audio::mixer
