#pragma once
#include "Component.hpp"

#include <string>
#include <memory>
#include <vector>

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
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		void takeFocus();
		void loseFocus();

	public:
		Parameter(std::string labelStr, std::string name, int x, int y, int fieldWidth);

	};
}
