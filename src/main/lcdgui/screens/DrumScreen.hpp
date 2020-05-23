#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class DrumScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int f) override;
		void turnWheel(int i) override;

		DrumScreen(const int layerIndex);
		void open() override;

		void setDrum(int i);
		int getDrum();

	private:
		void displayCurrentVal();
		void displayDrum();
		void displayPadToInternalSound();
		void displayPgm();
		void displayPgmChange();
		void displayMidiVolume();

		bool padToInternalSound = true;
		int drum = 0;

	public:
		void setPadToIntSound(bool b);
		bool isPadToIntSound();

	};
}
