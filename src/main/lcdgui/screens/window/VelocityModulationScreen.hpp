#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class VelocityModulationScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;

		VelocityModulationScreen(mpc::Mpc& mpc, const int layerIndex);
		
		void open() override;
		void close() override;

		void update(moduru::observer::Observable* observable, moduru::observer::Message message) override;

	private:
		void displayVeloAttack();
		void displayVeloStart();
		void displayVeloLevel();
		void displayVelo();
		void displayNote();

	};
}
