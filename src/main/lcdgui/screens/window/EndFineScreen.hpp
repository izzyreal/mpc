#pragma once

#include "lcdgui/ScreenComponent.hpp"

#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
    class EndFineScreen final : public ScreenComponent
    {

    public:
        EndFineScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void function(int i) override;
        void turnWheel(int i) override;
        void left() override;
        void right() override;
        void pressEnter() override;
        void setSlider(int) override;

    private:
        void displayEnd() const;
        void displayLngthLabel() const;
        void displaySmplLngth() const;

        void displayFineWave();
        void displayPlayX() const;

        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
    };
} // namespace mpc::lcdgui::screens::window
