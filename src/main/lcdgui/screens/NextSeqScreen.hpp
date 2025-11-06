#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class NextSeqScreen final : public ScreenComponent
    {

    public:
        NextSeqScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void turnWheel(int i) override;
        void function(int i) override;

    private:
        bool selectNextSqFromScratch = true;

        void displaySq() const;
        void displayNextSq() const;
        void displayNow0() const;
        void displayNow1() const;
        void displayNow2() const;
        void displayTempo() const;
        void displayTempoLabel() const;
        void displayTempoSource() const;
        void displayTiming() const;
    };
} // namespace mpc::lcdgui::screens
