#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class VeloEnvFilterScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;

		VeloEnvFilterScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		void close() override;

		void update(moduru::observer::Observable* observable, nonstd::any message);

	private:
		unsigned int velo = 127;
		void setVelo(int i);
		void displayVelo();
		void displayNote();
		void displayAttack();
		void displayDecay();
		void displayAmount();
		void displayVeloFreq();

	};
}
