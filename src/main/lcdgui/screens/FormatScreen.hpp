#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class FormatScreen
        : public mpc::lcdgui::ScreenComponent
    {
    public:
        void function(int i) override;

        FormatScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens
