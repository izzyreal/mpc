#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <string>
#include <chrono>

namespace mpc::lcdgui::screens::dialog2
{
    class PopupScreen final : public ScreenComponent
    {
    public:
        struct PopupBehavior
        {
            enum class OnTimeoutAction
            {
                None,         // persistent popup
                Close,        // close current screen
                OpenScreen,   // open another screen
                ReturnToLayer // unwind to specific layer
            };

            OnTimeoutAction action = OnTimeoutAction::None;
            std::optional<int> targetLayer;       // only used for ReturnToLayer
            std::optional<ScreenId> targetScreen; // only used for OpenScreen
        };

        struct PopupConfig
        {
            std::string text;
            bool closeUponButtonOrPadPressOrDataWheelTurn = false;
            std::optional<int> autoCloseDelayMs; // delay before timeout action
            PopupBehavior behavior; // what to do when timeout elapses
        };

        PopupScreen(Mpc &mpc, int layer);

        void onTimerCallback() override;

        void close() override;
        void show(const PopupConfig &config);
        void setText(std::string text);
        bool isCloseUponButtonOrPadPressOrDataWheelTurnEnabled() const;

    private:
        PopupConfig config;
        std::chrono::steady_clock::time_point startTime;
    };
} // namespace mpc::lcdgui::screens::dialog2
