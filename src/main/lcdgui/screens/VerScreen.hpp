#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class VerScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

	public:
		VerScreen(const int layerIndex);
	};
}
