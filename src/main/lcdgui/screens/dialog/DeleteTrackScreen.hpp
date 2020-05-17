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

		DeleteTrackScreen(const int layer);

		void open() override;

	private:
		int trackNumber = 0;
		void setTrackNumber(int i);
		void displayTrackNumber();

	};
}
