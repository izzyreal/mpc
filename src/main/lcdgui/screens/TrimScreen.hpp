#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class StartFineScreen;
    class EndFineScreen;
    class LoopToFineScreen;
    class LoopEndFineScreen;
    class ZoneStartFineScreen;
    class ZoneEndFineScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::lcdgui::screens
{
    class LoopScreen;
    class ZoneScreen;
} // namespace mpc::lcdgui::screens

namespace mpc::lcdgui::screens
{
    class TrimScreen final : public ScreenComponent
    {

    public:
        void openWindow() override;
        void function(int f) override;
        void turnWheel(int i) override;
        void left() override;
        void right() override;
        void pressEnter() override;
        void setSlider(int i) override;

        void setSliderStart(int) const;
        void setSliderEnd(int) const;
        void setEnd(int newValue) const;

    public:
        TrimScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        const std::vector<std::string> playXNames = {"ALL", "ZONE", "BEFOR ST",
                                                     "BEFOR TO", "AFTR END"};
        void displaySnd() const;
        void displayPlayX() const;
        void displaySt() const;
        void displayEnd() const;
        void displayView() const;
        void displayWave();

        bool smplLngthFix = false;
        int view = 0;

        void setView(int i);

    private:
        friend class LoopScreen;
        friend class ZoneScreen;
        friend class StartFineScreen;
        friend class EndFineScreen;
        friend class LoopEndFineScreen;
        friend class LoopToFineScreen;
        friend class ZoneStartFineScreen;
        friend class ZoneEndFineScreen;
    };
} // namespace mpc::lcdgui::screens
