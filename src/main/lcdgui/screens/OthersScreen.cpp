#include "OthersScreen.hpp"

using namespace mpc::lcdgui::screens;

OthersScreen::OthersScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "others", layerIndex)
{
}

void OthersScreen::open()
{
	displayTapAveraging();
}

void OthersScreen::displayTapAveraging()
{
	findField("tapaveraging")->setText(std::to_string(tapAveraging));
}

void OthersScreen::setTapAveraging(int i)
{
	if (i < 2 || i > 4)
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
	notifyObservers(std::string("contrast"));
}

int OthersScreen::getContrast()
{
	return contrast;
}

void OthersScreen::function(int i)
{
	init();
	switch (i)
	{
	case 1:
        mpc.getLayeredScreen()->openScreen<InitScreen>();
		break;
	case 2:
        mpc.getLayeredScreen()->openScreen<VerScreen>();
		break;
	}
}

void OthersScreen::turnWheel(int i)
{
	init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
	
	if (false /*mpc.getControls()->isAltPressed()*/)
	{
		auto increment = i > 0 ? 1 : -1;
		setContrast(contrast + increment);
	}
	else if (focusedFieldName == "tapaveraging")
	{
		setTapAveraging(tapAveraging + i);
	}
}
