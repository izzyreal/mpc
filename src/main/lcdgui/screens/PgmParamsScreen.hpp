#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class PgmParamsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:

		void function(int i) override;
		void turnWheel(int i) override;
		void openWindow() override;

		PgmParamsScreen(const int layerIndex);

	};
}
