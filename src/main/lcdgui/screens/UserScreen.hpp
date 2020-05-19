#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class UserScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		UserScreen(const int& layer);

		void open() override;

	private:
		void displayTempo();
		void displayLoop();
		void displayTsig();
		void displayBars();
		void displayPgm();
		void displayRecordingMode();
		void displayBus();
		void displayDeviceNumber();
		void displayVelo();
		void displayDeviceName();

	};
}
