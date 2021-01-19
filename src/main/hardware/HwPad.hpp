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
		std::weak_ptr<mpc::controls::BaseControls> controls;

	public:
		int getIndex();
		void push(int velo) override;
		void release() override;

	public:
		HwPad(mpc::Mpc& mpc, int index);

	};
}
