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

		void close() override;

		void returnToScreenAfterMilliSeconds(const std::string& screenName, const int delayInMs);
		void returnToScreenAfterInteraction(const std::string& screenName);

		void left() override { openScreen(returnToAfterInteractionScreen ); }
		void right() override { openScreen(returnToAfterInteractionScreen); }
		void up() override { openScreen(returnToAfterInteractionScreen); }
		void down() override { openScreen(returnToAfterInteractionScreen); }
		void function(int i) override { openScreen(returnToAfterInteractionScreen); }
		void openWindow() override { openScreen(returnToAfterInteractionScreen); }
		void turnWheel(int i) override { openScreen(returnToAfterInteractionScreen); }
		void numpad(int i) override { openScreen(returnToAfterInteractionScreen); }
		void pressEnter() override { openScreen(returnToAfterInteractionScreen); }
		void rec() override { openScreen(returnToAfterInteractionScreen); }
		void overDub() override { openScreen(returnToAfterInteractionScreen); }
		void stop() override { openScreen(returnToAfterInteractionScreen); }
		void play() override { openScreen(returnToAfterInteractionScreen); }
		void playStart() override { openScreen(returnToAfterInteractionScreen); }
		void mainScreen() override { openScreen(returnToAfterInteractionScreen); }
		void tap() override { openScreen(returnToAfterInteractionScreen); }
		void prevStepEvent() override { openScreen(returnToAfterInteractionScreen); }
		void nextStepEvent() override { openScreen(returnToAfterInteractionScreen); }
		void goTo() override { openScreen(returnToAfterInteractionScreen); }
		void prevBarStart() override { openScreen(returnToAfterInteractionScreen); }
		void nextBarEnd() override { openScreen(returnToAfterInteractionScreen); }
		void nextSeq() override { openScreen(returnToAfterInteractionScreen); }
		void trackMute() override { openScreen(returnToAfterInteractionScreen); }
		void bank(int i) override { openScreen(returnToAfterInteractionScreen); }
		void fullLevel() override { openScreen(returnToAfterInteractionScreen); }
		void sixteenLevels() override { openScreen(returnToAfterInteractionScreen); }
		void after() override { openScreen(returnToAfterInteractionScreen); }
		void shift() override { openScreen(returnToAfterInteractionScreen); }
		void undoSeq() override { openScreen(returnToAfterInteractionScreen); }
		void erase() override { openScreen(returnToAfterInteractionScreen); }

	private:
		std::thread returnToScreenThread;
		std::string returnToAfterInteractionScreen = "";
	};
}
