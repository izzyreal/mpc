#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class VmpcRecordJamScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

		VmpcRecordJamScreen(int layerIndex);

	};
}
