#include "TimeDisplayScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

TimeDisplayScreen::TimeDisplayScreen(const int layerIndex)
	: ScreenComponent("time-display", layerIndex)
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
	
	if (param.compare("displaystyle") == 0)
	{
		setDisplayStyle(displayStyle + i);
	}
	else if (param.compare("starttime") == 0)
	{
		setStartTime(startTime + i);
	}
	else if (param.compare("h") == 0)
	{
		setH(h + i);
	}
	else if (param.compare("m") == 0)
	{
		setM(m + i);
	}
	else if (param.compare("s") == 0)
	{
		setS(s + i);
	}
	else if (param.compare("f") == 0)
	{
		setF(f + i);
	}
	else if (param.compare("framerate") == 0)
	{
		setFrameRate(frameRate + i);
	}
}

void TimeDisplayScreen::displayDisplayStyle()
{
	findField("displaystyle").lock()->setText(displayStyleNames[displayStyle]);
}

void TimeDisplayScreen::displayStartTime()
{
	findField("starttime").lock()->setText(StrUtil::padLeft(to_string(startTime), "0", 2));
	findField("h").lock()->setText(StrUtil::padLeft(to_string(h), "0", 2));
	findField("m").lock()->setText(StrUtil::padLeft(to_string(m), "0", 2));
	findField("s").lock()->setText(StrUtil::padLeft(to_string(s), "0", 2));
	findField("f").lock()->setText(StrUtil::padLeft(to_string(f), "0", 2));
}

void TimeDisplayScreen::displayFrameRate()
{
	findField("framerate").lock()->setText(frameRateNames[frameRate]);
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
