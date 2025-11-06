#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoopSongScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

    public:
        LoopSongScreen(Mpc &mpc, int layerIndex);

    public:
        void open() override;

    private:
        void displayNumberOfSteps() const;
        void displayLastStep() const;
        void displayFirstStep() const;
    };
} // namespace mpc::lcdgui::screens::window
