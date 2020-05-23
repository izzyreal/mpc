#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class LoopScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		LoopScreen(const int layerIndex);

		void open() override;

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void pressEnter() override;
		void setSlider(int i) override;

	private:
		std::vector<std::string> playXNames = std::vector<std::string>{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		void displaySnd();
		void displayPlayX();
		void displayTo();
		void displayEndLength();
		void displayEndLengthValue();
		void displayLoop();

		void displayWave();

	};
}
