#pragma once

#include <observer/Observable.hpp>

namespace mpc {
	class Mpc;
}

namespace mpc::controls {
	class BaseControls;
}

namespace mpc::hardware {
	class HwPad
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		int index;
		std::weak_ptr<mpc::controls::BaseControls> controls;

	public:
		int getIndex();
		void push(int velo);
		void release();

	public:
		HwPad(mpc::Mpc& mpc, int index);

	};
}
