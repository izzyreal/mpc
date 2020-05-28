#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class ChannelSettingsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		ChannelSettingsScreen(const int layerIndex);
		
		void turnWheel(int increment) override;
		
		void open() override;
		void close() override;

		void update(moduru::observer::Observable* observable, nonstd::any message);

	private:
		const std::vector<std::string> fxPathNames{ "--", "M1", "M2", "R1", "R2" };
		const std::vector<std::string> stereoNamesSlash{ "-", "1/2", "1/2", "3/4", "3/4", "5/6", "5/6", "7/8", "7/8" };
		void displayNoteField();
		void displayStereoVolume();
		void displayIndividualVolume();
		void displayFxSendLevel();
		void displayPanning();
		void displayOutput();
		void displayFxPath();
		void displayFollowStereo();
		void displayChannel();

	};
}
