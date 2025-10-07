#pragma once
#include <Observer.hpp>

namespace mpc { class Mpc; }

namespace mpc::hardware {

	class Slider
		: public Observable
	{

	private:
		mpc::Mpc& mpc;
		int value = 50;

	public:
		Slider(mpc::Mpc& mpc);
		virtual void setValue(int i);
		int getValue();

	};
}
