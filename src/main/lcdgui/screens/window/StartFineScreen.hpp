#pragma once

#include "lcdgui/ScreenComponent.hpp"

#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
    class StartFineScreen final : public ScreenComponent
    {

    public:
        StartFineScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void function(int i) override;
        void turnWheel(int i) override;
        void left() override;
        void right() override;
        void pressEnter() override;
        void setSlider(int) override;

    private:
        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
        void displayFineWave();
        void displayStart() const;
        void displaySmplLngth() const;
        void displayLngthLabel() const;
        void displayPlayX() const;
    };
} // namespace mpc::lcdgui::screens::window
