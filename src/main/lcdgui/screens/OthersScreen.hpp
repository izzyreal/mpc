#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include "Observer.hpp"

namespace mpc::lcdgui::screens
{

    class OthersScreen final : public ScreenComponent, public Observable
    {

    private:
        void displayTapAveraging() const;
        int tapAveraging = 2;
        int contrast = 0;

    public:
        void setTapAveraging(int i);
        int getTapAveraging() const;
        void setContrast(int i);
        int getContrast() const;

        OthersScreen(Mpc &mpc, int layerIndex);
        void open() override;
        void function(int i) override;
        void turnWheel(int notch) override;
    };
} // namespace mpc::lcdgui::screens
