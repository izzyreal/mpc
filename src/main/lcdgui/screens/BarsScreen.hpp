#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class BarsScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int j) override;
        void turnWheel(int i) override;

        BarsScreen(mpc::Mpc &mpc, int layerIndex);
        void copyBars(int toSeqIndex, int copyFirstBar, int copyLastBar,
                      int copyCount, int copyAfterBar);

        void open() override;
        void close() override;

    private:
        void displayCopies();
        void displayToSq();
        void displayFromSq();
        void displayAfterBar();
        void displayLastBar();
        void displayFirstBar();

    private:
        int lastBar = 0;
        int firstBar = 0;
        int afterBar = 0;
        void setLastBar(int i, int max);
        void setFirstBar(int i, int max);
        void setAfterBar(int i, int max);
        void setCopies(int i);
    };
} // namespace mpc::lcdgui::screens
