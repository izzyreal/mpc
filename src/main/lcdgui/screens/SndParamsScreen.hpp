#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class SndParamsScreen final : public ScreenComponent
    {

    public:
        void openWindow() override;
        void function(int f) override;
        void turnWheel(int i) override;

        SndParamsScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
        void displayLevel() const;
        void displayTune() const;
        void displayBeat() const;
        void displaySampleAndNewTempo() const;
        void displaySnd() const;
        void displayPlayX() const;
    };
} // namespace mpc::lcdgui::screens
