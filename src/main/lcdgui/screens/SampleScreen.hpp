#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class SampleScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:

		void function(int i) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;

	public:
		SampleScreen(const int layerIndex);

	};
}
