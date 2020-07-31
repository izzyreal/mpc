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
		ls.lock()->openScreen("load");
		break;
	case 1:
		ls.lock()->openScreen("save");
		break;
	}
}
