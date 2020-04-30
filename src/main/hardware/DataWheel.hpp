#pragma once

#include <observer/Observable.hpp>

namespace mpc {

	

	namespace hardware {

		class DataWheel
			: public moduru::observer::Observable
		{

		private:
			

		public:
			void turn(int increment);

		public:
			DataWheel();
			~DataWheel();

		};

	}
}
