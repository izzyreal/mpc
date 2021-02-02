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
    display16LevelsEraseMode();
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
    else if (param.compare("16-levels-erase-mode") == 0)
    {
        set16LevelsEraseMode(_16LevelsEraseMode + i);
    }
}

void VmpcSettingsScreen::setInitialPadMapping(int i)
{
	if (i < 0 || i > 1)
		return;

	initialPadMapping = i;

	displayInitialPadMapping();
}

void VmpcSettingsScreen::displayInitialPadMapping()
{
	findField("initial-pad-mapping").lock()->setText(initialPadMappingNames[initialPadMapping]);
}

void VmpcSettingsScreen::display16LevelsEraseMode()
{
    findField("16-levels-erase-mode").lock()->setText(_16LevelsEraseModeNames[_16LevelsEraseMode]);
}

void VmpcSettingsScreen::set16LevelsEraseMode(int i)
{
    if (i < 0 || i > 1)
        return;

    _16LevelsEraseMode = i;
    display16LevelsEraseMode();
}
