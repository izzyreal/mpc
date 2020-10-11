#pragma once
#include "Component.hpp"

#include <string>
#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::lcdgui
{
	class Label;
	class Field;
}

namespace mpc::lcdgui
{
	class Parameter
		: public Component
	{

	public:
		Parameter(mpc::Mpc& mpc, std::string labelStr, std::string name, int x, int y, int fieldWidth);

	};
}
