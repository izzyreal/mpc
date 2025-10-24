#pragma once

#include <engine/audio/core/AudioProcessChain.hpp>
#include <engine/audio/mixer/AudioMixer.hpp>

#include <memory>

namespace mpc::engine::audio::mixer
{

    class AudioMixerStrip
        : public core::AudioProcessChain
    {

    public:
        AudioMixer *mixer{nullptr};

    private:
        mpc::engine::audio::core::AudioBuffer *buffer{nullptr};
        std::shared_ptr<AudioProcess> input;
        std::shared_ptr<AudioProcess> directOutput;
        bool isChannel{false};
        int nmixed{1};

    public:
        void setInputProcess(std::shared_ptr<AudioProcess> input);

        void setDirectOutputProcess(std::shared_ptr<AudioProcess> output);

        void silence();

    public:
        mpc::engine::audio::core::AudioBuffer *createBuffer();

    private:
        static const int silenceCount{500};
        int silenceCountdown;

    public:
        bool processBuffer(int nFrames);

        int mix(mpc::engine::audio::core::AudioBuffer *bufferToMix, std::vector<float> &gain);

    public:
        std::shared_ptr<AudioProcess> createProcess(std::shared_ptr<mpc::engine::audio::core::AudioControls> controls) override;

        void close() override;

    public:
        AudioMixerStrip(AudioMixer *mixer, std::shared_ptr<mpc::engine::audio::core::AudioControlsChain> controlsChain);
    };

} // namespace mpc::engine::audio::mixer
