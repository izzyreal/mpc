#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <string>

namespace mpc::lcdgui::screens::dialog2
{
    class PopupScreen final : public ScreenComponent
    {
    public:
        PopupScreen(Mpc &mpc, int layer);
        void setText(std::string text);
    };
} // namespace mpc::lcdgui::screens::dialog2
