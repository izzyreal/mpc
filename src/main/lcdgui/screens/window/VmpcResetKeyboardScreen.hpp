#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcResetKeyboardScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcResetKeyboardScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
