#pragma once

#include <observer/Observable.hpp>

#include <string>

namespace mpc {
	class Mpc;
	namespace hardware {

		class Pot
			//: public moduru::observer::Observable
		{

		private:
			std::string label{ "" };
			int value{ 0 };
			mpc::Mpc* mpc;

		public:
			std::string getLabel();
			void setValue(int i);
			int getValue();

		public:
			Pot(std::string label, mpc::Mpc* mpc);
			~Pot();

		};

	}
}
