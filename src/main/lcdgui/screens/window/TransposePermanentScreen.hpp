#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class TransposePermanentScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        void function(int i) override;

    public:
        TransposePermanentScreen(mpc::Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
