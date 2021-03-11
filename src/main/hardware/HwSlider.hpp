#pragma once
#include <observer/Observable.hpp>

namespace mpc { class Mpc; }

namespace mpc::hardware {

	class Slider
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		int value = 50;

	public:
		Slider(mpc::Mpc& mpc);
		void setValue(int i);
		int getValue();

	};
}
