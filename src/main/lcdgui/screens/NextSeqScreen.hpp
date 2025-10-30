#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class NextSeqScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        NextSeqScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

        void turnWheel(int i) override;
        void function(int i) override;

    private:
        bool selectNextSqFromScratch = true;

        void displaySq();
        void displayNextSq();
        void displayNow0();
        void displayNow1();
        void displayNow2();
        void displayTempo();
        void displayTempoLabel();
        void displayTempoSource();
        void displayTiming();
    };
} // namespace mpc::lcdgui::screens
