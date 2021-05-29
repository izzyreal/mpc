#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class KeepOrRetryScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
        KeepOrRetryScreen(mpc::Mpc& mpc, const int layerIndex);

        void update(moduru::observer::Observable* o, nonstd::any arg) override;

        void open() override;
        void close() override;
		void function(int i) override;
		void turnWheel(int i) override;
		void mainScreen() override;
        void right() override;

	private:
		void displayNameForNewSound();
		void displayAssignToNote();
        void openNameScreen();

	};
}
