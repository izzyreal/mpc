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
		EndFineScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		
		void function(int i) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void pressEnter() override;
        void setSlider(int) override;

	private:
		void displayEnd();
		void displayLngthLabel();
		void displaySmplLngth();

		void displayFineWave();
		void displayPlayX();
		
		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };

	};
}
