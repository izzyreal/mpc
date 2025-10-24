#pragma once

#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
    class StartFineScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        StartFineScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

        void function(int i) override;
        void turnWheel(int i) override;
        void left() override;
        void right() override;
        void pressEnter() override;
        void setSlider(int) override;

    private:
        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END"};
        void displayFineWave();
        void displayStart();
        void displaySmplLngth();
        void displayLngthLabel();
        void displayPlayX();
    };
} // namespace mpc::lcdgui::screens::window
