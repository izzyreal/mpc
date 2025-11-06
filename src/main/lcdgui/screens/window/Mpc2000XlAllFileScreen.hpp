#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class Mpc2000XlAllFileScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        Mpc2000XlAllFileScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
