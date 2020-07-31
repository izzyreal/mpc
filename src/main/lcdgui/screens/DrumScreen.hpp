#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class MixerScreen;
}

namespace mpc::controls
{
	class BaseSamplerControls;
}

namespace mpc::lcdgui::screens
{
	class DrumScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int f) override;
		void turnWheel(int i) override;

		DrumScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;

	private:
		void displayCurrentVal();
		void displayDrum();
		void displayPadToInternalSound();
		void displayPgm();
		void displayPgmChange();
		void displayMidiVolume();

		void setDrum(int i);

		bool padToInternalSound = true;
		int drum = 0;

	public:
		void setPadToIntSound(bool b);
		bool isPadToIntSound();

	private:
		friend class MixerScreen;
		friend class MixerSetupScreen;
		friend class SelectDrumScreen;
		friend class SelectMixerDrumScreen;
		friend class mpc::controls::BaseSamplerControls;

	};
}
