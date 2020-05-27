#pragma once

#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
	class EndFineScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		EndFineScreen(const int layerIndex);

		void open() override;
		
		void function(int i) override;
		void turnWheel(int i) override;


	private:
		void displayEnd();
		void displayLngthLabel();
		void displaySmplLngth();

		void displayFineWaveform();
		void displayPlayX();
		
		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };

	};
}
