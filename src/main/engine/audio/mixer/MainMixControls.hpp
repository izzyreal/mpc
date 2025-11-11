#pragma once

#include "engine/audio/mixer/MixControls.hpp"
#include "engine/control/EnumControl.hpp"

namespace mpc::engine::audio::mixer
{

    class MainMixControls : public MixControls
    {

        control::EnumControl *routeControl{nullptr};

    public:
        control::EnumControl *createRouteControl(int stripId) override;

        control::EnumControl *getRouteControl() const;

        MainMixControls(MixerControls *mixerControls, int stripId,
                        const std::shared_ptr<BusControls> &busControls,
                        bool isMaster);
    };

} // namespace mpc::engine::audio::mixer
