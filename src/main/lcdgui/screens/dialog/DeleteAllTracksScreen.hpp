#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

	class DeleteAllTracksScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

		DeleteAllTracksScreen(const int layerIndex);

	};
}
