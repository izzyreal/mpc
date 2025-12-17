#pragma once

#include "engine/audio/core/AudioControls.hpp"
#include "engine/FaderControl.hpp"
#include "engine/audio/mixer/BusControls.hpp"
#include "engine/audio/mixer/MixerControls.hpp"
#include "engine/control/EnumControl.hpp"

#include <atomic>

namespace mpc::engine::audio::mixer
{
    class LCRControl;

    class MixControls : public core::AudioControls
    {
        static float HALF_ROOT_TWO_;

        std::shared_ptr<control::BooleanControl> soloControl;
        std::shared_ptr<control::BooleanControl> muteControl;
        std::shared_ptr<FaderControl> gainControl;
        std::shared_ptr<LCRControl> lcrControl;
        std::shared_ptr<BusControls> busControls;

    protected:
        MixerControls *mixerControls{nullptr};

    public:
        MixerControls *getMixerControls() const;

    private:
        bool master{false};
        int channelCount{0};
        bool mute{false};
        std::atomic<float> gain{0};
        float left{0}, right{0};

    public:
        void derive(Control *c) override;

        bool isMaster() const;

        bool isMute() const;

        bool isEnabled() const;

        float getGain() const;

        void getChannelGains(std::vector<float> *dest) const;

        float getSmoothingFactor() const;

        virtual control::EnumControl *createRouteControl(int stripId);

        virtual control::BooleanControl *createMuteControl();

        MixControls(MixerControls *mixerControls, int stripId,
                    const std::shared_ptr<BusControls> &busControls,
                    bool isMaster);

        std::string getName() override;

        static float &HALF_ROOT_TWO();
    };
} // namespace mpc::engine::audio::mixer
