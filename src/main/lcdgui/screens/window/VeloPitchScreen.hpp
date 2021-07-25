#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class VeloPitchScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;

		VeloPitchScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		void close() override;

		void update(moduru::observer::Observable* observable, nonstd::any message) override;

	private:
		void displayVelo();
		void displayNote();
		void displayVeloPitch();
		void displayTune();
	};
}
