#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class DrumControls
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int f) override;
		void turnWheel(int i) override;

		DrumControls(const int layerIndex);

	};
}
