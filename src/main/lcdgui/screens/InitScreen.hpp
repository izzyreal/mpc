#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class InitScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        InitScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens
