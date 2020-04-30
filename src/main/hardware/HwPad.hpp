#pragma once

#include <observer/Observable.hpp>

namespace mpc {

	

	namespace controls {
		class BaseControls;
	}

	namespace hardware {

		class HwPad
			: public moduru::observer::Observable
		{

		private:
			
			int index;
			std::weak_ptr<mpc::controls::BaseControls> controls;

		public:
			int getIndex();
			void push(int velo);
			void release();

		public:
			HwPad(int index);
			~HwPad();

		};

	}
}
