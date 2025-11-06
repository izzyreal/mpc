#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class InitScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;

        InitScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens
