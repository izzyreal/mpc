#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class LoopScreen final : public ScreenComponent
    {
    public:
        LoopScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void openWindow() override;
        void function(int f) override;
        void turnWheel(int i) override;
        void left() override;
        void right() override;
        void pressEnter() override;
        void setSlider(int i) override;

        void setSliderLoopTo(int) const;
        void setSliderLength(int) const;
        void setLoopTo(int) const;
        void setLength(int) const;

        bool isLoopLengthFixed() const;
        void setLoopLengthFixed(bool);

    private:
        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
        void displaySnd() const;
        void displayPlayX() const;
        void displayTo() const;
        void displayEndLength() const;
        void displayEndLengthValue() const;
        void displayLoop() const;
        void displayWave();

        bool endSelected = true;
        bool loopLngthFix = false;

        void setEndSelected(bool b);
    };
} // namespace mpc::lcdgui::screens
