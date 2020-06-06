#include "VerScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

VerScreen::VerScreen(const int layerIndex) 
	: ScreenComponent("ver", layerIndex)
{
}

void VerScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 0:
		ls.lock()->openScreen("others");
		break;
	case 1:
		ls.lock()->openScreen("init");
		break;
	}
}
