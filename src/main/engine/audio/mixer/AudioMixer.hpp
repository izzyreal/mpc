#pragma once

#include <engine/audio/server/AudioServer.hpp>
#include <engine/audio/server/AudioClient.hpp>
#include <engine/audio/mixer/MixControls.hpp>

#include <vector>

namespace ctoot::audio::mixer {

    class AudioMixerStrip;

    class AudioMixerBus;

    class AudioMixer final
            : public ctoot::audio::server::AudioClient
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
        std::vector<std::shared_ptr<AudioMixerStrip>> groupStrips;
        std::vector<std::shared_ptr<AudioMixerStrip>> auxStrips;
        std::shared_ptr<AudioMixerStrip> mainStrip;
        std::shared_ptr<ctoot::audio::server::AudioServer> server;
        ctoot::audio::core::AudioBuffer *sharedAudioBuffer = nullptr;

    public:
        std::shared_ptr<MixerControls> getMixerControls();

        std::shared_ptr<ctoot::audio::server::AudioServer> getAudioServer();

    public:
        ctoot::audio::core::AudioBuffer *getSharedBuffer();

        ctoot::audio::core::AudioBuffer *createBuffer(std::string name);

        void removeBuffer(ctoot::audio::core::AudioBuffer *buffer);

    public:
        std::shared_ptr<AudioMixerStrip> getStrip(std::string name);

        std::shared_ptr<AudioMixerStrip> getStripImpl(std::string name);

        void work(int nFrames) override;

    private:
        void evaluateStrips(std::vector<std::shared_ptr<AudioMixerStrip>> *evalStrips, int nFrames);

        void silenceStrips(std::vector<std::shared_ptr<AudioMixerStrip>> *evalStrips);

        void writeBusBuffers(int nFrames);

        void createBusses(std::shared_ptr<MixerControls> mixerControls);

        std::shared_ptr<AudioMixerBus> createBus(std::shared_ptr<BusControls> busControls);

    public:
        std::shared_ptr<AudioMixerBus> getBus(std::string name);

        std::shared_ptr<AudioMixerBus> getMainBus();

        std::shared_ptr<AudioMixerStrip> getMainStrip();

    public:
        void createStrips(std::shared_ptr<MixerControls> mixerControls);

        std::shared_ptr<AudioMixerStrip> createStrip(std::shared_ptr<ctoot::audio::core::AudioControlsChain> controls);

    public:
        void close();

    public:
        AudioMixer(std::shared_ptr<MixerControls> controls, std::shared_ptr<ctoot::audio::server::AudioServer> server);

    };
}
