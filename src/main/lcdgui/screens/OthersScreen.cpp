#include "OthersScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

OthersScreen::OthersScreen(const int layerIndex) 
	: ScreenComponent("others", layerIndex)
{
}

void OthersScreen::open()
{
	displayTapAveraging();
}

void OthersScreen::displayTapAveraging()
{
	findField("tapaveraging").lock()->setText(to_string(tapAveraging));
}

void OthersScreen::setTapAveraging(int i)
{
	if (i < 0 || i > 8)
	{
		return;
	}
	tapAveraging = i;
	displayTapAveraging();
}

int OthersScreen::getTapAveraging()
{
	return tapAveraging;
}

void OthersScreen::setContrast(int i)
{
	if (i < 0 || i > 50)
	{
		return;
	}
	contrast = i;
	// Currently does not affect anything
}

void OthersScreen::function(int i)
{
	init();
	switch (i)
	{
	case 1:
		ls.lock()->openScreen("init");
		break;
	case 2:
		ls.lock()->openScreen("ver");
		break;
	}
}

void OthersScreen::turnWheel(int i)
{
	init();
	auto controls = Mpc::instance().getControls().lock();
	
	if (mpc::Mpc::instance().getControls().lock()->isAltPressed())
	{
		setContrast(contrast + i);
	}
	else if (param.compare("tapaveraging") == 0)
	{
		setTapAveraging(tapAveraging + i);
	}
}
