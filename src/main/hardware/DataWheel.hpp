#pragma once

#include <observer/Observable.hpp>

#include <functional>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
	class DataWheel
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;

	public:
		DataWheel(mpc::Mpc& mpc);
        std::function<void(int)> updateUi = [](int increment){};
        void turn(int increment);

	};
}
