#pragma once

#include "engine/audio/mixer/AudioMixerStrip.hpp"

#include <memory>

namespace mpc::sampler
{
    class Sound;
    class Sampler;
} // namespace mpc::sampler

namespace mpc::engine
{
    class Voice;

    class PreviewSoundPlayer final
    {

    private:
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::shared_ptr<mpc::engine::Voice> voice;
        std::shared_ptr<mpc::sampler::Sound> tempVars;

        std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<mpc::engine::FaderControl> fader;

        bool soundHasLoop = false;

        PreviewSoundPlayer(const PreviewSoundPlayer &other) = delete;
        PreviewSoundPlayer &operator=(const PreviewSoundPlayer &other) = delete;

    public:
        void finishVoice() const;

        void finishVoiceIfSoundIsLooping() const;

        void playSound(int soundNumber, int velocity, int frameOffset);

        void connectVoice() const;

        explicit PreviewSoundPlayer(
            std::shared_ptr<mpc::sampler::Sampler> sampler,
            std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer,
            std::shared_ptr<Voice> voice);
    };
} // namespace mpc::engine
