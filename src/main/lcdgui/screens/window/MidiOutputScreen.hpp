#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class MidiOutputScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;
        void right() override;

	public:
		MidiOutputScreen(mpc::Mpc& mpc, const int layerIndex);

	public:
		void open() override;

	private:
		int softThru = 0;
		int deviceIndex = 1;
		std::vector<std::string> softThruNames = { "OFF", "AS TRACK", "OMNI-A", "OMNI-B", "OMNI-AB" };
		void displaySoftThru();
		void displayDeviceName();
		void setSoftThru(int i);
		void setDeviceIndex(int i);
        void openNameScreen();

	public:
		int getSoftThru();

	};
}
