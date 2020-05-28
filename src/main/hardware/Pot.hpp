#pragma once

#include <string>

namespace mpc::hardware {

	class Pot
	{

	private:
		std::string label = "";
		int value{ 0 };


	public:
		std::string getLabel();
		void setValue(int i);
		int getValue();

	public:
		Pot(std::string label);

	};
}
