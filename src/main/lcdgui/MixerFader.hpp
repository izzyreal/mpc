#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{
	class MixerFader
		: public Component
	{

	private:
		int value{ 100 };
		bool color = true;

	public:
		virtual void setValue(int value);
		virtual void setColor(bool on);

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		MixerFader(MRECT rect);

	};
}
