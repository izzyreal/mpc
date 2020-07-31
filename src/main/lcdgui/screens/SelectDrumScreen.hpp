#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class SelectDrumScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

		SelectDrumScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	};
}
