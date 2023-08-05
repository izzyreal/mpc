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
		void update(moduru::observer::Observable* o, moduru::observer::Message message) override;

	private:
		void displayNote();
		void displayNote0();
		void displayNote1();

	};
}
