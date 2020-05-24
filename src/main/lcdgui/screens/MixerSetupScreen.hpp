#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class MixerSetupScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		MixerSetupScreen(const int& layer);
		void turnWheel(int increment) override;
		void function(int i) override;
		void open() override;

	private:
		void displayMasterLevel();
		void displayFxDrum();
		void displayStereoMixSource();
		void displayIndivFxSource();
		void displayCopyPgmMixToDrum();
		void displayRecordMixChanges();

	};
}
