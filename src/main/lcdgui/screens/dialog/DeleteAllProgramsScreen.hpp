#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class DeleteAllProgramsScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
		void function(int j) override;

		DeleteAllProgramsScreen(mpc::Mpc& mpc, const int layerIndex);

	};
}
