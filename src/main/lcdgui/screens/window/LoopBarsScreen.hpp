#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class LoopBarsScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        LoopBarsScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayNumberOfBars() const;
        void displayLastBar() const;
        void displayFirstBar() const;
    };
} // namespace mpc::lcdgui::screens::window
