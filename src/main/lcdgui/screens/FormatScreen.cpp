#include "FormatScreen.hpp"

using namespace mpc::lcdgui::screens;

FormatScreen::FormatScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "format", layerIndex)
{
}

void FormatScreen::function(int i)
{
	init();

	switch (i)
	{
	case 0:
		openScreen("load");
		break;
	case 1:
		openScreen("save");
		break;
	}
}
