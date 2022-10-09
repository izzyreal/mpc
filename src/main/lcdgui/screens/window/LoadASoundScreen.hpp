#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class LoadASoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		void keepSound();

	public:
		LoadASoundScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		void close() override;

        // Block uncontrolled ways to leave this screen
        void mainScreen() override;
        void numpad(int i) override {}

		void update(moduru::observer::Observable* observable, nonstd::any message) override;

		void turnWheel(int i) override;
		void function(int i) override;

	private:
		void displayAssignToNote();
        unsigned int assignToNote = 60;

	};
}
