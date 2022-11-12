#include "LoadAProgramScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadAProgramScreen::LoadAProgramScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "load-a-program", layerIndex)
{
}

void LoadAProgramScreen::open()
{
	displayLoadReplaceSound();
}

void LoadAProgramScreen::turnWheel(int i)
{
	init();

	if (param.compare("load-replace-sound") == 0)
	{
		loadReplaceSound = i > 0;
		displayLoadReplaceSound();
	}
}

void LoadAProgramScreen::function(int i)
{
	init();

    auto selectedFile = mpc.screens->get<LoadScreen>("load")->getSelectedFile();
    
	switch (i)
	{
	case 2:
	{
		clearProgramWhenLoading = true;
		auto oldLoadReplaceSound = loadReplaceSound;
		loadReplaceSound = true;
        mpc.getDisk().lock()->readPgm2(selectedFile);
		loadReplaceSound = oldLoadReplaceSound;
		break;
	}
	case 3:
		openScreen("load");
		break;
	case 4:
		clearProgramWhenLoading = false;
        mpc.getDisk().lock()->readPgm2(selectedFile);
		break;
	}
}

void LoadAProgramScreen::displayLoadReplaceSound()
{
	findField("load-replace-sound").lock()->setText(std::string(loadReplaceSound ? "YES" : "NO(FASTER)"));
}
