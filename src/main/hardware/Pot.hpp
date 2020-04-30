#pragma once

#include <observer/Observable.hpp>

#include <string>

namespace mpc {
	
	namespace hardware {

		class Pot
			//: public moduru::observer::Observable
		{

		private:
			std::string label{ "" };
			int value{ 0 };
			

		public:
			std::string getLabel();
			void setValue(int i);
			int getValue();

		public:
			Pot(std::string label);
			~Pot();

		};

	}
}
