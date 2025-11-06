#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class FormatScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;

        FormatScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens
