#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class SndParamsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:

		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;

		SndParamsScreen(const int layerIndex);
		void open() override;

	private:
		std::vector<std::string> playXNames = vector<string>{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		void displayLevel();
		void displayTune();
		void displayBeat();
		void displaySampleAndNewTempo();
		void displaySnd();
		void displayPlayX();

	};
}
