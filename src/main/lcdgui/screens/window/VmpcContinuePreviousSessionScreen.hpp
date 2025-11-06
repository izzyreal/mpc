#pragma once

#include <functional>

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcContinuePreviousSessionScreen final : public ScreenComponent
    {
    public:
        VmpcContinuePreviousSessionScreen(Mpc &, int layer);

        void setRestoreAutoSavedStateAction(const std::function<void()> &);

        void function(int) override;

    private:
        std::function<void()> restoreAutoSavedStateAction;
    };
} // namespace mpc::lcdgui::screens::window
