#pragma once

#include "lcdgui/ScreenComponent.hpp"

#include <cstdint>

namespace mpc::lcdgui::screens
{
    class TransScreen final : public ScreenComponent
    {
    public:
        static constexpr int8_t ALL_TRACKS = -1;
        TransScreen(Mpc &mpc, int layerIndex);
        void function(int i) override;
        void turnWheel(int increment) override;

        void open() override;

        void play() override;
        void playStart() override;
        void rec() override;
        void overDub() override;
        void stop() override;

        void setTransposeAmount(int i);
        int getTransposeAmount() const;
        int getTr() const;
        int getBar0() const;
        int getBar1() const;

    private:
        int8_t tr = ALL_TRACKS;
        int transposeAmount = 0;
        int bar0 = 0;
        int bar1 = 0;

        void setTr(int8_t i);
        void setBar0(int i);
        void setBar1(int i);

        void displayTransposeAmount() const;
        void displayTr() const;
        void displayBars() const;
    };
} // namespace mpc::lcdgui::screens
