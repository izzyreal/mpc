#pragma once

#include "engine/audio/core/AudioProcessChain.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"

#include <memory>

namespace mpc::engine::audio::mixer
{
    class AudioMixerStrip : public core::AudioProcessChain
    {

    public:
        AudioMixer *mixer{nullptr};

    private:
        std::weak_ptr<core::AudioBuffer> buffer;
        std::shared_ptr<AudioProcess> input;
        std::shared_ptr<AudioProcess> directOutput;
        bool isChannel{false};
        int nmixed{1};

    public:
        void setInputProcess(const std::shared_ptr<AudioProcess> &);

        void setDirectOutputProcess(const std::shared_ptr<AudioProcess> &);

        void silence();

        std::shared_ptr<core::AudioBuffer> createBuffer();

    private:
        static const int silenceCount{500};
        int silenceCountdown;

    public:
        bool processBuffer(int nFrames);

        int mix(core::AudioBuffer *bufferToMix, const std::vector<float> &gain);

        std::shared_ptr<AudioProcess>
        createProcess(std::shared_ptr<core::AudioControls> controls) override;

        void close() override;

        AudioMixerStrip(
            AudioMixer *mixer,
            const std::shared_ptr<core::AudioControlsChain> &controlsChain);
    };

} // namespace mpc::engine::audio::mixer
