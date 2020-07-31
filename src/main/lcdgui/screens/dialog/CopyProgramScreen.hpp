#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class CopyProgramScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
		void function(int i) override;
		void turnWheel(int i) override;

		CopyProgramScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		int pgm0 = 0;
		int pgm1 = 0;

		void setPgm0(int i);
		void setPgm1(int i);

		void displayPgm0();
		void displayPgm1();

	};
}
