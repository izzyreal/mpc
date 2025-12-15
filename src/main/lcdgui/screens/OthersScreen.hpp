#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include "Observer.hpp"

namespace mpc::lcdgui::screens
{
    class OthersScreen final : public ScreenComponent, public Observable
    {
    public:
        OthersScreen(Mpc &mpc, int layerIndex);
        void open() override;
        void function(int i) override;
        void turnWheel(int notch) override;

        void setTapAveraging(int i);
        int getTapAveraging() const;
        void setContrast(int i);
        int getContrast() const;

    private:
        void displayTapAveraging() const;
        void displayContrast() const;
        int tapAveraging = 2;
        int contrast = 0;
    };
} // namespace mpc::lcdgui::screens
