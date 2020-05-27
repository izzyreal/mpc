#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class LoopEndFineScreen;
	class LoopToFineScreen;
}

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
		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		void displaySnd();
		void displayPlayX();
		void displayTo();
		void displayEndLength();
		void displayEndLengthValue();
		void displayLoop();
		void displayWave();

		bool endSelected = true;
		bool loopLngthFix = false;

		void setEndSelected(bool b);

	private:
		friend class mpc::lcdgui::screens::window::LoopEndFineScreen;
		friend class mpc::lcdgui::screens::window::LoopToFineScreen;

	};
}
