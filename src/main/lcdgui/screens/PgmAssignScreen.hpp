#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class PgmAssignScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:

		void function(int i) override;
		void turnWheel(int i) override;
		void openWindow() override;

		PgmAssignScreen(const int layerIndex);

	};
}
