#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class VmpcRecordJamScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

		VmpcRecordJamScreen(mpc::Mpc& mpc, int layerIndex);

	};
}
