#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class ChangeBarsScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        void open() override;

    private:
        void displayAfterBar() const;
        void displayNumberOfBars() const;
        void displayFirstBar() const;
        void displayLastBar() const;

    public:
        ChangeBarsScreen(Mpc &mpc, int layerIndex);

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
