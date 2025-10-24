#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class Mpc2000XlAllFileScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        Mpc2000XlAllFileScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
