#pragma once

#include <observer/Observable.hpp>

namespace mpc::hardware {

	class DataWheel
		: public moduru::observer::Observable
	{

	public:
		void turn(int increment);

	};
}
