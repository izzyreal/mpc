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
		PopupScreen(mpc::Mpc& mpc, int layer);
		~PopupScreen();

		void close() override;

		void returnToScreenAfterMilliSeconds(const std::string& screenName, const int delayInMs);
		void setScreenToReturnTo(const std::string& screenName);

	private:
		std::thread returnToScreenThread;
		std::string screenToReturnTo;
	};
}
