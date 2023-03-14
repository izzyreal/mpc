#pragma once

#include <engine/audio/core/AudioControls.hpp>
#include <engine/audio/mixer/MixVariables.hpp>
#include <engine/mpc/FaderControl.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixerControls.hpp>
#include <engine/control/EnumControl.hpp>

namespace ctoot::audio::mixer {

    class LCRControl;

    class MixControls
            : public MixVariables, public ctoot::audio::core::AudioControls
    {

    private:
        static float HALF_ROOT_TWO_;

    private:
        std::shared_ptr<ctoot::control::BooleanControl> soloControl;
        std::shared_ptr<ctoot::control::BooleanControl> muteControl;
        std::shared_ptr<ctoot::mpc::FaderControl> gainControl;
        std::shared_ptr<LCRControl> lcrControl;
        std::shared_ptr<BusControls> busControls;

    protected:
        MixerControls *mixerControls{nullptr};

    public:
        MixerControls *getMixerControls();

    private:
        bool master{false};
        int channelCount{0};
        bool mute{false};
        float gain{0}, left{0}, right{0};

    public:
        void derive(Control *c) override;

    public:
        bool isMaster() override;

        std::shared_ptr<ctoot::audio::core::ChannelFormat> getChannelFormat() override;

        bool isMute();

        bool isEnabled() override;

        float getGain() override;

        void getChannelGains(std::vector<float> *dest) override;

        float getSmoothingFactor() override;

    public:
        virtual ctoot::control::EnumControl *createRouteControl(int stripId);

        virtual ctoot::control::BooleanControl *createMuteControl();

    public:
        MixControls(MixerControls *mixerControls, int stripId, std::shared_ptr<BusControls> busControls, bool isMaster);

    public:
        virtual std::string getName() override;

    public:
        static float &HALF_ROOT_TWO();
    };
}
