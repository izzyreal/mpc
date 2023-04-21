#pragma once
#include "Component.hpp"
#include <string>
#include <thread>

namespace mpc::lcdgui {

	class Background
		: public Component
	{

	private:
		MRECT unobtrusiveRect;
        unsigned short scrollOffset = 0;
        bool scrolling = false;
        bool scrollingDown = false;
        std::unique_ptr<std::thread> scrollThread;

	public:
		void repaintUnobtrusive(MRECT rect); // This is used for hiding function keys of windows and dialogs
		void setName(const std::string& name);
		void Draw(std::vector< std::vector<bool>>* pixels) override;
        void setScrolling(bool);
		Background();

	};
}
