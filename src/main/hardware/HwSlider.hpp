#pragma once

#include <observer/Observable.hpp>

namespace mpc {
	
	namespace hardware {

		class Slider
			: public moduru::observer::Observable
		{

		private:
			
			int value = 0;

		public:
			void setValue(int i);
			int getValue();

		public:
			Slider();
			~Slider();

		};

	}
}
