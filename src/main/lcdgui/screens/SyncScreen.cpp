#include "SyncScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

SyncScreen::SyncScreen(const int layerIndex) 
	: ScreenComponent("sync", layerIndex)
{
}

void SyncScreen::open()
{
	displayIn();
	displayOut();
	displayModeIn();
	displayModeOut();
	displayReceiveMMC();
	displaySendMMC();
}

void SyncScreen::turnWheel(int i)
{
	init();

	if (param.compare("in") == 0)
	{
		setIn(in + i);
	}
	else if (param.compare("out") == 0)
	{
		setOut(out + i);
	}
	else if (param.compare("mode-in") == 0)
	{
		setModeIn(getModeIn() + i);
	}
	else if (param.compare("mode-out") == 0)
	{
		setModeOut(getModeOut() + i);
	}
	else if (param.compare("receive-mmc") == 0)
	{
		setReceiveMMCEnabled(i > 0);
	}
	else if (param.compare("send-mmc") == 0)
	{
		setSendMMCEnabled(i > 0);
	}
}

void SyncScreen::function(int i)
{
	init();

	switch (i)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
}

void SyncScreen::setIn(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	in = i;
	displayIn();
}

void SyncScreen::setOut(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	out = i;
	displayOut();
}

void SyncScreen::setShiftEarly(int i)
{
	shiftEarly = i;
	//displayShiftEarly();
}

void SyncScreen::setSendMMCEnabled(bool b)
{
	sendMMCEnabled = b;
	displaySendMMC();
}

void SyncScreen::setFrameRate(int i)
{
	frameRate = i;
	//displayFrameRate();
}

void SyncScreen::setModeIn(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	modeIns[in] = i;
	displayModeIn();
}

int SyncScreen::getModeOut()
{
	if (out > 1) 
	{
		return modeOuts[0];
	}
	return modeOuts[out];
}

void SyncScreen::setModeOut(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	if (out <= 1)
	{
		modeOuts[out] = i;
	}
	else
	{
		modeOuts[0] = i;
		modeOuts[1] = i;
	}
	displayModeOut();
}

void SyncScreen::setReceiveMMCEnabled(bool b)
{
	receiveMMCEnabled = b;
	displayReceiveMMC();
}

void SyncScreen::displayIn()
{
	string result = to_string(in + 1);
	findField("in").lock()->setText(result);
}

void SyncScreen::displayOut()
{
	auto outText = " A";

	if (out == 1)
	{
		outText = " B";
	}
	else if (out == 2)
	{
		outText = "A/B";
	}

	findField("out").lock()->setText(outText);
}

void SyncScreen::displayModeIn()
{
	findField("mode-in").lock()->setText(modeNames[getModeIn()]);
}

void SyncScreen::displayModeOut()
{
	findField("mode-out").lock()->setText(modeNames[getModeOut()]);
}

void SyncScreen::displayReceiveMMC()
{
	auto mmc = string(receiveMMCEnabled ? "ON" : "OFF");
	findField("receive-mmc").lock()->setText(mmc);
}

void SyncScreen::displaySendMMC()
{
	auto mmc = string(sendMMCEnabled ? "ON" : "OFF");
	findField("send-mmc").lock()->setText(mmc);
}

int SyncScreen::getModeIn()
{
	return modeIns[in];
}
