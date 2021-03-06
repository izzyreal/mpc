#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class MuteAssignScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;

		MuteAssignScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;
		void close() override;
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	private:
		void displayNote();
		void displayNote0();
		void displayNote1();

	};
}
