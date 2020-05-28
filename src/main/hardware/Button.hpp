#pragma once

#include <observer/Observable.hpp>

#include <string>

namespace mpc {

	

	namespace controls {
		class BaseControls;
	}

	namespace hardware {

		class Button
			: public moduru::observer::Observable
		{

		private:
			
			std::string label = "";
			std::weak_ptr<mpc::controls::BaseControls> controls;

		public:
			std::string getLabel();
			void push();
			void release();

		public:
			Button(std::string label);
			~Button();

		};

	}
}
