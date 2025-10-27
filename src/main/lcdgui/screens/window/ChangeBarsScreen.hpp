#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class ChangeBarsScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

    public:
        void open() override;

    private:
        void displayAfterBar();
        void displayNumberOfBars();
        void displayFirstBar();
        void displayLastBar();

    public:
        ChangeBarsScreen(mpc::Mpc &mpc, const int layerIndex);

    private:
        int lastBar = 0;
        int firstBar = 0;
        int numberOfBars = 0;
        int afterBar = 0;

        void setLastBar(int i);
        void setFirstBar(int i);
        void setNumberOfBars(int i);
        void setAfterBar(int i);
    };
} // namespace mpc::lcdgui::screens::window
