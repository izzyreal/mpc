#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class VerScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        VerScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens
