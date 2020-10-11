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
		void repaintUnobtrusive(MRECT rect); // This is used for hiding function keys of windows and dialogs
		void setName(const std::string& name);
		void Draw(std::vector< std::vector<bool>>* pixels) override;
		Background();

	};
}
