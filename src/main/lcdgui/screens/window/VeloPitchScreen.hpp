#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseSamplerControls.hpp>

namespace mpc::lcdgui::screens::window
{
	class VeloPitchScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::controls::BaseSamplerControls
	{

	public:
		void turnWheel(int i) override;

		VeloPitchScreen(const int layerIndex);

		void open() override;
		void close() override;

		void update(moduru::observer::Observable* observable, nonstd::any message);

	private:
		void displayVelo();
		void displayNote();
		void displayVeloPitch();
		void displayTune();
	};
}
