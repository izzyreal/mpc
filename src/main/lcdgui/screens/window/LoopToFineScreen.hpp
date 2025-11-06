#pragma once

#include <lcdgui/ScreenComponent.hpp>

#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
    class LoopToFineScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        LoopToFineScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;

        void function(int i) override;
        void turnWheel(int i) override;
        void left() override;
        void right() override;
        void pressEnter() override;
        void setSlider(int) override;

    private:
        void displayLoopLngth();
        void displayLngthField();
        void displayFineWave();
        void displayPlayX();
        void displayTo();

        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
    };
} // namespace mpc::lcdgui::screens::window
