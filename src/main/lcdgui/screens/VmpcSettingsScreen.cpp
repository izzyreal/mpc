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
    displayAutoConvertWavs();
}

void VmpcSettingsScreen::function(int i)
{
    switch (i)
    {
        case 1:
            openScreen("vmpc-keyboard");
            break;
        case 2:
            openScreen("vmpc-auto-save");
            break;
        case 3:
            openScreen("vmpc-disks");
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
    else if (param.compare("auto-convert-wavs") == 0)
    {
        setAutoConvertWavs(autoConvertWavs + i);
    }
}

void VmpcSettingsScreen::setInitialPadMapping(int i)
{
	if (i < 0 || i > 2)
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

void VmpcSettingsScreen::setAutoConvertWavs(int i)
{
    if (i < 0 || i > 1)
        return;

    autoConvertWavs = i;
    displayAutoConvertWavs();
}

void VmpcSettingsScreen::displayAutoConvertWavs()
{
    findField("auto-convert-wavs").lock()->setText(autoConvertWavs == 1 ? "YES" : "NO");
}
