#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class ChangeBars2Screen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

    public:
        ChangeBars2Screen(Mpc &mpc, int layerIndex);

    public:
        void open() override;

    private:
        int newBars = 1;
        void setNewBars(int i);

    private:
        void displayCurrent() const;
        void displayNewBars() const;
    };
} // namespace mpc::lcdgui::screens::window
