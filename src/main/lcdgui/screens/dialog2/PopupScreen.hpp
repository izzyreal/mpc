#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <string>

namespace mpc::lcdgui::screens::dialog2
{
    class PopupScreen final : public ScreenComponent
    {
    public:
        PopupScreen(Mpc &mpc, int layer);
        void close() override;
        void setText(std::string text);
        void setCloseUponButtonOrPadPressOrDataWheelTurn(const bool);
        bool isCloseUponButtonOrPadPressOrDataWheelTurnEnabled() const;

    private:
        bool closeUponButtonOrPadPressOrDataWheelTurn = false;
    };
} // namespace mpc::lcdgui::screens::dialog2
