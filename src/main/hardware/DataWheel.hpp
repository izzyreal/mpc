#pragma once

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
        virtual void turn(int increment, const bool updateUiEnabled = true);

	};
}
