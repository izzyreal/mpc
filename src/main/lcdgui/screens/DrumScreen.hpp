#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class DrumScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int f) override;
		void turnWheel(int i) override;

		DrumScreen(const int layerIndex);

	};
}
