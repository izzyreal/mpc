#pragma once

#include <observer/Observable.hpp>

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
		void turn(int increment);

	};
}
