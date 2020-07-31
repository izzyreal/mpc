#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseSamplerControls.hpp>

namespace mpc::lcdgui::screens::window
{
	class ProgramScreen
		: public mpc::lcdgui::ScreenComponent
		//public mpc::controls::BaseSamplerControls
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		ProgramScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;

	private:
		void displayProgramName();
		void displayMidiProgramChange();

	};
}
