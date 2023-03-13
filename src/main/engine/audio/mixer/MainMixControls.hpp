#pragma once

#include <engine/audio/mixer/MixVariables.hpp>
#include <engine/audio/mixer/MixControls.hpp>
#include <engine/control/EnumControl.hpp>

namespace ctoot::audio::mixer {

    class MainMixControls
            : public MixControls
    {

    private:
        ctoot::control::EnumControl *routeControl{nullptr};

    public:
        ctoot::control::EnumControl *createRouteControl(int stripId) override;

        ctoot::control::EnumControl *getRouteControl() override;

    public:
        MainMixControls(MixerControls *mixerControls, int stripId, std::shared_ptr<BusControls> busControls,
                        bool isMaster);

        ~MainMixControls();

    };

}
