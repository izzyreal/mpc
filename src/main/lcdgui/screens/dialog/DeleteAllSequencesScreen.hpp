#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

	class DeleteAllSequencesScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

		DeleteAllSequencesScreen(const int layerIndex);

	};
}
