#include "CantFindFileScreen.hpp"

using namespace mpc::lcdgui::screens::window;

CantFindFileScreen::CantFindFileScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "cant-find-file", layerIndex)
{
}

void CantFindFileScreen::open()
{
	findField("file")->setText(fileName);
}

void CantFindFileScreen::function(int i)
{
	init();

	switch (i)
	{
	case 1:
		skipAll = true;
		waitingForUser = false;
		break;
	case 2:
		waitingForUser = false;
		break;
	}
}
