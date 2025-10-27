#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class LoopSongScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;

    public:
        LoopSongScreen(mpc::Mpc &mpc, const int layerIndex);

    public:
        void open() override;

    private:
        void displayNumberOfSteps();
        void displayLastStep();
        void displayFirstStep();
    };
} // namespace mpc::lcdgui::screens::window
