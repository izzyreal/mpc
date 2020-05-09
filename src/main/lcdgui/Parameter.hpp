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

	private:
		std::shared_ptr<Field> tf;
		std::shared_ptr<Label> label;

	public:
		std::weak_ptr<Field> getTf();
		std::weak_ptr<Label> getLabel();

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		Parameter(std::string labelStr, std::string name, int x, int y, int size);
		~Parameter();

	};
}
