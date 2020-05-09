#pragma once
#include <string>
#include<memory>

#include "Component.hpp"

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
		Parameter(std::string labelStr, std::string name, int x, int y, int size);
		~Parameter();

	};
}
