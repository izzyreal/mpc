#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class MidiSwScreen final : public ScreenComponent
    {
    public:
        MidiSwScreen(Mpc &, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

    private:
        void displaySwitchLabels();
        void displayCtrlsAndFunctions();
    };
} // namespace mpc::lcdgui::screens
