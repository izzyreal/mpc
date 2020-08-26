#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

	class DeleteAllSongScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

		DeleteAllSongScreen(mpc::Mpc& mpc, const int layerIndex);

	};
}
