#pragma once

#include <memory>
#include <cstdint>

#include <hardware/HwComponent.hpp>
#include <Observer.hpp>

namespace mpc {
    class Mpc;
}

namespace mpc::controls {
    class BaseControls;
}

namespace mpc::hardware {
    class HwPad : public HwComponent, public Observable
    {

    private:
        int index;
        std::weak_ptr<mpc::controls::BaseControls> controls;
        unsigned char pressure = 0;
        char padIndexWithBankWhenLastPressed = -1;

    public:
        int getIndex();
        char getPadIndexWithBankWhenLastPressed();
        void setPadIndexWithBankWhenLastPressed(char padIndexWithBank);
        void push(int velo) override;
        void release() override;
        virtual bool isPressed();
        virtual void setPressure(uint8_t newPressure);
        unsigned char getPressure();

    public:
        HwPad(mpc::Mpc& mpc, int index);

    };
}
