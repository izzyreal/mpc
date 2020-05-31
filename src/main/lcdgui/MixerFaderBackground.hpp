#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{

	class MixerFaderBackground
		: public Component
	{

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		MixerFaderBackground(MRECT rect);
		void setColor(bool);

	private:
		bool color = true;

	};
}
