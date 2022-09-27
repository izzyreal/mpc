#pragma once

#include <hardware/HwComponent.hpp>
#include <observer/Observable.hpp>

namespace mpc {
	class Mpc;
}

namespace mpc::controls {
	class BaseControls;
}

namespace mpc::hardware {
	class HwPad
		: public HwComponent
        , public moduru::observer::Observable
	{

	private:
		int index;
    bool down = false;
    unsigned char pressure = 0;

	public:
		int getIndex();
		void push(int velo) override;
		void release() override;
    bool isDown();
    unsigned char getPressure();
    void setPressure(unsigned char);

	public:
		HwPad(mpc::Mpc& mpc, int index);

	};
}
