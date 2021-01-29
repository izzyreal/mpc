#include "VmpcSettingsScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

VmpcSettingsScreen::VmpcSettingsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-settings", layerIndex)
{
}

void VmpcSettingsScreen::open()
{
	displayInitialPadMapping();
}

void VmpcSettingsScreen::function(int i)
{
    switch (i)
    {
        case 1:
            openScreen("vmpc-keyboard");
            break;
    }
}

void VmpcSettingsScreen::turnWheel(int i)
{
    init();

	if (param.compare("initial-pad-mapping") == 0)
	{
		setInitialPadMapping(initialPadMapping + i);
	}
}

void VmpcSettingsScreen::setInitialPadMapping(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}

	initialPadMapping = i;

	displayInitialPadMapping();
}

void VmpcSettingsScreen::displayInitialPadMapping()
{
	findField("initial-pad-mapping").lock()->setText(initialPadMappingNames[initialPadMapping]);
}
