#pragma once
#include "Component.hpp"

#include <memory>
#include <vector>

namespace mpc::lcdgui
{
	class EnvGraph
		: public Component
	{

	private:
		std::vector<std::vector<int>> coordinates{};

	public:
		void setCoordinates(std::vector<std::vector<int>> ia);
		void setCoordinates(int attack, int decay);

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		EnvGraph();

	};
}
