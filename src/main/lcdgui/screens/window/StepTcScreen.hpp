#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class StepTcScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		StepTcScreen(const int layerIndex);
		void open() override;

	private:
		std::vector<std::string> timingCorrectNames = { "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };
		void displayTcValue();
	};
}
