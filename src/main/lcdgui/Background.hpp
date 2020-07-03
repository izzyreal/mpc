#pragma once
#include "Component.hpp"
#include <string>

namespace mpc::lcdgui {

	class Background
		: public Component
	{

	public:
		void setName(const std::string& name);

	public:
		void Draw(std::vector< std::vector<bool>>* pixels) override;

	public:
		Background();

	};
}
