#pragma once

#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
	class LoopToFineScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		LoopToFineScreen(const int layerIndex);

		void open() override;
		
		void function(int i) override;
		void turnWheel(int i) override;

	private:
		void displayLoopLngth();
		void displayLngthField();
		void displayFineWaveform();
		void displayPlayX();
		void displayTo();

		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };

	};
}
