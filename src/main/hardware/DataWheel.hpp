#pragma once

#include <functional>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
	class DataWheel
	{

	private:
		mpc::Mpc& mpc;

	public:
		DataWheel(mpc::Mpc& mpc);
        std::function<void(int)> updateUi = [](int increment){};
        void turn(int increment, const bool updateUiEnabled = true);

	};
}
