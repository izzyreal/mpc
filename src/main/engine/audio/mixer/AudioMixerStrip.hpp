#pragma once

#include <engine/audio/core/AudioProcessChain.hpp>
#include <engine/audio/mixer/AudioMixer.hpp>

#include <memory>

namespace ctoot::audio::mixer {

    class AudioMixerStrip
            : public core::AudioProcessChain
    {

    public:
        AudioMixer *mixer{nullptr};

    private:
        ctoot::audio::core::AudioBuffer *buffer{nullptr};
        std::shared_ptr<AudioProcess> input;
        std::shared_ptr<AudioProcess> directOutput;
        bool isChannel{false};
        std::shared_ptr<ctoot::audio::core::ChannelFormat> channelFormat;
        int nmixed{1};

    public:

        void setInputProcess(std::shared_ptr<AudioProcess> input);

        void setDirectOutputProcess(std::shared_ptr<AudioProcess> output);

        void silence();

    public:
        ctoot::audio::core::AudioBuffer *createBuffer();

    private:
        static const int silenceCount{1000};
        int silenceCountdown{};

    public:
        bool processBuffer(int nFrames);

        virtual int mix(ctoot::audio::core::AudioBuffer *bufferToMix, std::vector<float> &gain);

    public:
        std::shared_ptr<AudioProcess> createProcess(std::shared_ptr<ctoot::audio::core::AudioControls> controls) override;

        void close() override;

    public:
        AudioMixerStrip(AudioMixer *mixer, std::shared_ptr<ctoot::audio::core::AudioControlsChain> controlsChain);

        ~AudioMixerStrip();

    };

}
