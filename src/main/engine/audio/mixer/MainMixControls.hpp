#pragma once

#include <engine/audio/mixer/MixVariables.hpp>
#include <engine/audio/mixer/MixControls.hpp>
#include <engine/control/EnumControl.hpp>

namespace mpc::engine::audio::mixer {

    class MainMixControls
            : public MixControls
    {

    private:
        mpc::engine::control::EnumControl *routeControl{nullptr};

    public:
        mpc::engine::control::EnumControl *createRouteControl(int stripId) override;

        mpc::engine::control::EnumControl *getRouteControl() override;

    public:
        MainMixControls(MixerControls *mixerControls, int stripId, std::shared_ptr<BusControls> busControls,
                        bool isMaster);

    };

}
