#pragma once

#include "HwComponent.hpp"
#include <Observer.hpp>

#include <functional>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
	class DataWheel
		: public Observable
	{

	private:
		mpc::Mpc& mpc;

	public:
		DataWheel(mpc::Mpc& mpc);
        std::function<void(int)> updateUi = [](int increment){};
        void turn(int increment);

	};

    class DataWheelUp : public HwComponent {
    public:
        DataWheelUp(mpc::Mpc& mpc, DataWheel& dataWheelToUse)
        : HwComponent(mpc, "datawheel-up"), dataWheel(dataWheelToUse) {}
        void push(int) override;
    private:
        DataWheel& dataWheel;
    };

    class DataWheelDown : public HwComponent {
    public:
        DataWheelDown(mpc::Mpc& mpc, DataWheel& dataWheelToUse)
                : HwComponent(mpc, "datawheel-down"), dataWheel(dataWheelToUse) {}
        void push(int) override;
    private:
        DataWheel& dataWheel;
    };
}
