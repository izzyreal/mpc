#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class KeepOrRetryScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;
		void mainScreen() override;

	public:
		KeepOrRetryScreen(const int layerIndex);

		void update(moduru::observer::Observable* o, nonstd::any arg) override;

		void open() override;
		void close() override;

	private:
		void displayNameForNewSound();
		void displayAssignToNote();

	};
}
