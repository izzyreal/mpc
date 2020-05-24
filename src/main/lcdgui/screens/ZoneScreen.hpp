#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class ZoneScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;

		ZoneScreen(const int layerIndex);

		void open() override;
		
	private:
		void displayWave();
		void displaySnd();
		void displayPlayX();
		void displaySt();
		void displayEnd();
		void displayZone();

	private:
		const std::vector<std::string> playXNames = { "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };

	};
}
