#pragma once

#include <engine/control/EnumControl.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>

namespace mpc::engine::audio::mixer {

    class RouteControl
            : public mpc::engine::control::EnumControl
    {

    public:
        RouteControl(MainMixControls *mmc, std::string defaultRoute, bool canRouteToGroups);

        friend class MainMixControls;
    };

}
