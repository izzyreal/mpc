#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

	class DeleteTrackScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		DeleteTrackScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		int trackNumber = 0;
		void setTrackNumber(int i);
		void displayTrackNumber();

	};
}
