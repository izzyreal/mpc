#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

	class DeleteSongScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		DeleteSongScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		void displaySong();

	};
}
