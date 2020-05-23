#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class InitPadAssignScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		void setInitPadAssign(bool b);
		void displayInitPadAssign();
		bool initPadAssignIsMaster = false;

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		InitPadAssignScreen(const int layerIndex);

		void open() override;

	};
}
