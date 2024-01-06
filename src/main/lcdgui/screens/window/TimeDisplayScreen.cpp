#include "TimeDisplayScreen.hpp"

using namespace mpc::lcdgui::screens::window;

TimeDisplayScreen::TimeDisplayScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "time-display", layerIndex)
{
}

void TimeDisplayScreen::open()
{
	displayDisplayStyle();
	displayStartTime();
	displayFrameRate();
}

void TimeDisplayScreen::turnWheel(int i)
{
	init();
	
	if (param == "displaystyle")
	{
		setDisplayStyle(displayStyle + i);
	}
	else if (param == "starttime")
	{
		setStartTime(startTime + i);
	}
	else if (param == "h")
	{
		setH(h + i);
	}
	else if (param == "m")
	{
		setM(m + i);
	}
	else if (param == "s")
	{
		setS(s + i);
	}
	else if (param == "f")
	{
		setF(f + i);
	}
	else if (param == "framerate")
	{
		setFrameRate(frameRate + i);
	}
}

void TimeDisplayScreen::displayDisplayStyle()
{
	findField("displaystyle")->setText(displayStyleNames[displayStyle]);
}

void TimeDisplayScreen::displayStartTime()
{
	findField("starttime")->setText(StrUtil::padLeft(std::to_string(startTime), "0", 2));
	findField("h")->setText(StrUtil::padLeft(std::to_string(h), "0", 2));
	findField("m")->setText(StrUtil::padLeft(std::to_string(m), "0", 2));
	findField("s")->setText(StrUtil::padLeft(std::to_string(s), "0", 2));
	findField("f")->setText(StrUtil::padLeft(std::to_string(f), "0", 2));
}

void TimeDisplayScreen::displayFrameRate()
{
	findField("framerate")->setText(frameRateNames[frameRate]);
}

void TimeDisplayScreen::setDisplayStyle(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	displayStyle = i;
	displayDisplayStyle();
}

void TimeDisplayScreen::setH(int i)
{
	if (i < 0 || i > 59)
	{
		return;
	}
	h = i;
	displayStartTime();
}

void TimeDisplayScreen::setStartTime(int i)
{
	if (i < 0 || i > 23) {
		return;
	}

	startTime = i;
	displayStartTime();
}

void TimeDisplayScreen::setM(int i)
{
	if (i < 0 || i > 59) {
		return;
	}
	m = i;
	displayStartTime();
}

void TimeDisplayScreen::setF(int i)
{
	if (i < 0 || i > 99) {
		return;
	}
	f = i;
	displayStartTime();
}

void TimeDisplayScreen::setS(int i)
{
	if (i < 0 || i > 29) {
		return;
	}
	s = i;
	displayStartTime();
}

void TimeDisplayScreen::setFrameRate(int i)
{
	if (i < 0 || i > 3) {
		return;
	}
	frameRate = i;
	displayFrameRate();
}

int TimeDisplayScreen::getDisplayStyle()
{
    return displayStyle;
}