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
		UserScreen(const int layerIndex);
		
		void update(moduru::observer::Observable* o, nonstd::any arg) override;
		void open() override;
		void close() override;

	private:
		void displayTempo();
		void displayLoop();
		void displayTsig();
		void displayBars();
		void displayPgm();
		void displayRecordingMode();
		void displayTrackType();
		void displayDeviceNumber();
		void displayVelo();
		void displayDeviceName();

		std::vector<std::string> busNames = { "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };
	};
}
