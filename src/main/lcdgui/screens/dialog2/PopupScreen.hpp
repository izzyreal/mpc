#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <string>
#include <thread>

namespace mpc::lcdgui::screens::dialog2
{
	class PopupScreen
		: public ScreenComponent
	{

	public:
		void setText(std::string text);

	public:
		PopupScreen(mpc::Mpc& mpc);
		~PopupScreen();

		void returnToScreenAfterMilliSeconds(const std::string& screenName, const int delayInMs);

	private:
		std::thread returnToScreenThread;
	};
}
