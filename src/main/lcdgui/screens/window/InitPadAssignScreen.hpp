#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseSamplerControls.hpp>

namespace mpc::lcdgui::screens::window {

	class InitPadAssignScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::controls::BaseSamplerControls
	{

	private:
		void displayInitPadAssign();
		bool initPadAssignIsMaster = false;

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		InitPadAssignScreen(const int layerIndex);

		void open() override;

	};
}
