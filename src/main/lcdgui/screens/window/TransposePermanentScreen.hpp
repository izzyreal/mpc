#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class TransposePermanentScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;

        TransposePermanentScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
