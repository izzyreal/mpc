#pragma once

#include <functional>

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window {
    class VmpcContinuePreviousSessionScreen : public ScreenComponent {
    public:
        VmpcContinuePreviousSessionScreen(mpc::Mpc&, int layer);

        void setRestoreAutoSavedStateAction(std::function<void()>);

        void function(int) override;
        void open() override;

    private:
        std::function<void()> restoreAutoSavedStateAction;
    };
}