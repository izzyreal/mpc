#pragma once

#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
	class LoopEndFineScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		LoopEndFineScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		
		void function(int i) override;
		void turnWheel(int i) override;


	private:
		void displayLoopLngth();
		void displayLngthField();
		void displayFineWaveform();
		void displayEnd();
		void displayPlayX();

		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };

	};
}
