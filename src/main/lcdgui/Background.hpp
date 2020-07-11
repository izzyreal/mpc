#pragma once
#include "Component.hpp"
#include <string>

namespace mpc::lcdgui {

	class Background
		: public Component
	{

	private:
		MRECT unobtrusiveRect;

	public:
		void repaintUnobtrusive(MRECT rect); // This is used for disappearing function keys
		void setName(const std::string& name);
		void Draw(std::vector< std::vector<bool>>* pixels) override;
		Background();

	};
}
