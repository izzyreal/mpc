#include "LoadAProgramScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace std;

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
	
	switch (i)
	{
	case 2:
	{
		clearProgramWhenLoading = true;
		auto oldLoadReplaceSound = loadReplaceSound;
		loadReplaceSound = true;
		mpc.loadProgram();
		loadReplaceSound = oldLoadReplaceSound;
		break;
	}
	case 3:
		openScreen("load");
		break;
	case 4:
		clearProgramWhenLoading = false;
		mpc.loadProgram();
		break;
	}
}

void LoadAProgramScreen::displayLoadReplaceSound()
{
	findField("load-replace-sound").lock()->setText(string(loadReplaceSound ? "YES" : "NO(FASTER)"));
}
