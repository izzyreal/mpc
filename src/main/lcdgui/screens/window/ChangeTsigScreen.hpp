#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <sequencer/TimeSignature.hpp>

namespace mpc::lcdgui::screens::window
{

    class ChangeTsigScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        ChangeTsigScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayBars() const;
        void displayNewTsig() const;

        sequencer::TimeSignature timesignature;
        int bar0 = 0;
        int bar1 = 0;

        void setBar0(int i, int max);
        void setBar1(int i, int max);
    };
} // namespace mpc::lcdgui::screens::window
