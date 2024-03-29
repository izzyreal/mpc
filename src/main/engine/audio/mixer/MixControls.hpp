#pragma once

#include <engine/audio/core/AudioControls.hpp>
#include "engine/FaderControl.hpp"
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixerControls.hpp>
#include <engine/control/EnumControl.hpp>

namespace mpc::engine::audio::mixer {

    class LCRControl;

    class MixControls : public mpc::engine::audio::core::AudioControls
    {

    private:
        static float HALF_ROOT_TWO_;

    private:
        std::shared_ptr<mpc::engine::control::BooleanControl> soloControl;
        std::shared_ptr<mpc::engine::control::BooleanControl> muteControl;
        std::shared_ptr<mpc::engine::FaderControl> gainControl;
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
        bool isMaster();

        bool isMute();

        bool isEnabled();

        float getGain();

        void getChannelGains(std::vector<float> *dest);

        float getSmoothingFactor();

    public:
        virtual mpc::engine::control::EnumControl *createRouteControl(int stripId);

        virtual mpc::engine::control::BooleanControl *createMuteControl();

    public:
        MixControls(MixerControls *mixerControls, int stripId, std::shared_ptr<BusControls> busControls, bool isMaster);

    public:
        std::string getName() override;

    public:
        static float &HALF_ROOT_TWO();
    };
}
