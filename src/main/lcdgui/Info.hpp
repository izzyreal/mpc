#pragma once
#include "Label.hpp"

#include "Component.hpp"

#include <memory>
#include <string>

namespace mpc::lcdgui {

	class Info
		: public Component
	{

	public:
		Info(const std::string& name, int x, int y, int size);

		void Draw(std::vector<std::vector<bool>>* pixels) override;

	};
}
