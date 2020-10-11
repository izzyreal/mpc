#pragma once

#include <string>

namespace mpc { class Mpc; }

namespace mpc::hardware {

	class Pot
	{
	private:
		mpc::Mpc& mpc;
		std::string label = "";
		int value{ 0 };


	public:
		std::string getLabel();
		void setValue(int i);
		int getValue();

	public:
		Pot(mpc::Mpc& mpc, std::string label);

	};
}
