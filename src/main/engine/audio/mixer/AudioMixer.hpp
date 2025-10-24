#pragma once

#include <engine/audio/server/AudioServer.hpp>
#include <engine/audio/server/AudioClient.hpp>
#include <engine/audio/mixer/MixControls.hpp>

#include <vector>

namespace mpc::engine::audio::mixer
{

    class AudioMixerStrip;

    class AudioMixerBus;

    class AudioMixer final
        : public mpc::engine::audio::server::AudioClient
    {

    private:
        std::shared_ptr<MixerControls> controls;

    protected:
        std::shared_ptr<AudioMixerBus> mainBus;
        std::vector<std::shared_ptr<AudioMixerBus>> busses;
        std::vector<std::shared_ptr<AudioMixerBus>> auxBusses;

    private:
        std::vector<std::shared_ptr<AudioMixerStrip>> strips;
        std::vector<std::shared_ptr<AudioMixerStrip>> channelStrips;
        std::vector<std::shared_ptr<AudioMixerStrip>> auxStrips;
        std::shared_ptr<AudioMixerStrip> mainStrip;
        std::shared_ptr<mpc::engine::audio::server::AudioServer> server;
        mpc::engine::audio::core::AudioBuffer *sharedAudioBuffer = nullptr;

    public:
        std::shared_ptr<MixerControls> getMixerControls();

    public:
        mpc::engine::audio::core::AudioBuffer *getSharedBuffer();

        mpc::engine::audio::core::AudioBuffer *createBuffer(std::string name);

        void removeBuffer(mpc::engine::audio::core::AudioBuffer *buffer);

    public:
        std::shared_ptr<AudioMixerStrip> getStrip(std::string name);

        std::shared_ptr<AudioMixerStrip> getStripImpl(std::string name);

        void work(int nFrames) override;

    private:
        static void evaluateStrips(std::vector<std::shared_ptr<AudioMixerStrip>> &stripsToEvaluate, int nFrames);

        static void silenceStrips(std::vector<std::shared_ptr<AudioMixerStrip>> &stripsToSilence);

        void writeBusBuffers(int nFrames);

        void createBusses(std::shared_ptr<MixerControls> mixerControls);

        std::shared_ptr<AudioMixerBus> createBus(std::shared_ptr<BusControls> busControls);

    public:
        std::shared_ptr<AudioMixerBus> getBus(std::string name);

        std::shared_ptr<AudioMixerBus> getMainBus();

        std::shared_ptr<AudioMixerStrip> getMainStrip();

    public:
        void createStrips(std::shared_ptr<MixerControls> mixerControls);

        std::shared_ptr<AudioMixerStrip> createStrip(std::shared_ptr<mpc::engine::audio::core::AudioControlsChain> controls);

    public:
        void close();

    public:
        AudioMixer(const std::shared_ptr<MixerControls> &controls, const std::shared_ptr<mpc::engine::audio::server::AudioServer> &server);
    };
} // namespace mpc::engine::audio::mixer
