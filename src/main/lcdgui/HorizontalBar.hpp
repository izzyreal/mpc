#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{
	class HorizontalBar
		: public Component
	{

	private:
		int value;

	public:
		void setValue(int value);
		void Draw(std::vector<std::vector<bool>>* pixels) override;

		HorizontalBar(MRECT rect, int value = 0);

	};
}
