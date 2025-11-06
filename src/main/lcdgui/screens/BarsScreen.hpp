#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class BarsScreen final : public ScreenComponent
    {

    public:
        void function(int j) override;
        void turnWheel(int i) override;

        BarsScreen(Mpc &mpc, int layerIndex);
        void copyBars(int toSeqIndex, int copyFirstBar, int copyLastBar,
                      int copyCount, int copyAfterBar) const;

        void open() override;
        void close() override;

    private:
        void displayCopies() const;
        void displayToSq() const;
        void displayFromSq() const;
        void displayAfterBar() const;
        void displayLastBar() const;
        void displayFirstBar() const;

    private:
        int lastBar = 0;
        int firstBar = 0;
        int afterBar = 0;
        void setLastBar(int i, int max);
        void setFirstBar(int i, int max);
        void setAfterBar(int i, int max);
        void setCopies(int i) const;
    };
} // namespace mpc::lcdgui::screens
