#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class SndParamsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:

		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;

		SndParamsScreen(const int layerIndex);

	};
}
