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
		std::weak_ptr<Field> getField();
		std::weak_ptr<Label> getLabel();

	public:
		Parameter(std::string labelStr, std::string name, int x, int y, int size);
		~Parameter();

	};
}
