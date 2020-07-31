#include "VerScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

VerScreen::VerScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "ver", layerIndex)
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
