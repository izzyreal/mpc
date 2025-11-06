#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::lcdgui::screens::window
{
    class TransposePermanentScreen;
}

namespace mpc::lcdgui::screens
{
    class TransScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        void open() override;

        TransScreen(Mpc &mpc, int layerIndex);

        void play() override;
        void playStart() override;
        void rec() override;
        void overDub() override;
        void stop() override;

    private:
        int tr = -1;
        int transposeAmount = 0;
        int bar0 = 0;
        int bar1 = 0;

        void setTransposeAmount(int i);
        void setTr(int i);
        void setBar0(int i);
        void setBar1(int i);

        void displayTransposeAmount() const;
        void displayTr() const;
        void displayBars() const;

        friend class TransposePermanentScreen;
        friend class audiomidi::EventHandler;
    };
} // namespace mpc::lcdgui::screens
