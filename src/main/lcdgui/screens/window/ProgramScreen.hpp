#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class ProgramScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		ProgramScreen(const int layerIndex);
		void open() override;

	private:
		void displayProgramName();
		void displayMidiProgramChange();

	};
}
