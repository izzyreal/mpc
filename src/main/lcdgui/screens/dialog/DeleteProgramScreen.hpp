#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class DeleteProgramScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
		void function(int i) override;
		void turnWheel(int i) override;

		DeleteProgramScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;

	private:
		int pgm = 0;

		void setDeletePgm(int i);
		void displayPgm();

	};
}
