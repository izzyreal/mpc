#pragma once
#include "Component.hpp"

namespace mpc::lcdgui {

	class PunchRect
		: public Component
	{

	private:
		bool on = false;

	public:
		void setOn(bool newOn);

	public:
		void Draw(std::vector< std::vector<bool>>* pixels) override;

	public:
		PunchRect(const std::string& name, MRECT rect);

	};
}
