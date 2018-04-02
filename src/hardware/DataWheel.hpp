#pragma once

#include <observer/Observable.hpp>

namespace mpc {

	class Mpc;

	namespace hardware {

		class DataWheel
			: public moduru::observer::Observable
		{

		private:
			mpc::Mpc* mpc;

		public:
			void turn(int increment);

		public:
			DataWheel(mpc::Mpc* mpc);
			~DataWheel();

		};

	}
}
