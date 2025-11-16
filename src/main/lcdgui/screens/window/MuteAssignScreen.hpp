#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class MuteAssignScreen final : public ScreenComponent
    {
    public:
        MuteAssignScreen(Mpc &mpc, int layerIndex);
        void turnWheel(int i) override;
        void open() override;

    private:
        void displayNote() const;
        void displayNote0() const;
        void displayNote1() const;
    };
} // namespace mpc::lcdgui::screens::window
