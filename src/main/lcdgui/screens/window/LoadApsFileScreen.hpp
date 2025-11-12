#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <disk/ApsLoader.hpp>

namespace mpc::lcdgui::screens::window
{
    class LoadApsFileScreen final : public ScreenComponent
    {

        std::unique_ptr<disk::ApsLoader> apsLoader;

    public:
        void function(int i) override;

        LoadApsFileScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
