#include "IgnoreTempoChangeScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

IgnoreTempoChangeScreen::IgnoreTempoChangeScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "ignore-tempo-change", layerIndex)
{
}

void IgnoreTempoChangeScreen::open()
{
	findField("insequence")->setAlignment(Alignment::Centered);
	displayIgnore();
}

void IgnoreTempoChangeScreen::function(int i)
{
	switch (i)
	{
	case 3:
		openScreen("song");
		break;
	}
}

void IgnoreTempoChangeScreen::turnWheel(int i)
{
	ignore = i > 0;
	displayIgnore();
}

void IgnoreTempoChangeScreen::displayIgnore()
{
	findField("insequence")->setText(ignore ? "ON" : "OFF");
}

void IgnoreTempoChangeScreen::setIgnore(bool b)
{
    ignore = b;
}

bool IgnoreTempoChangeScreen::getIgnore()
{
    return ignore;
}