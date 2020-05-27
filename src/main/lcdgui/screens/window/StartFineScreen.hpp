#pragma once

#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
	class StartFineScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		StartFineScreen(const int layerIndex);

		void open() override;
		
		void function(int i) override;
		void turnWheel(int i) override;

	private:
		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		void displayFineWaveform();
		void displayStart();
		void displaySmplLngth();
		void displayLngthLabel();
		void displayPlayX();

	};
}
