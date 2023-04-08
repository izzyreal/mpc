#include "CountMetronomeScreen.hpp"

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::window;

CountMetronomeScreen::CountMetronomeScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "count-metronome", layerIndex)
{
}

void CountMetronomeScreen::open()
{
	findField("in-play")->setAlignment(Alignment::Centered);
	findField("in-rec")->setAlignment(Alignment::Centered);
	findField("wait-for-key")->setAlignment(Alignment::Centered);

	displayCountIn();
	displayInPlay();
	displayRate();
	displayInRec();
	displayWaitForKey();
}

void CountMetronomeScreen::displayWaitForKey()
{
	findField("wait-for-key")->setText(waitForKey ? "ON" : "OFF");
}

void CountMetronomeScreen::displayInRec()
{
	findField("in-rec")->setText(inRec ? "YES" : "NO");
}

void CountMetronomeScreen::displayRate()
{
	findField("rate")->setText(rateNames[rate]);
}

void CountMetronomeScreen::displayInPlay()
{
	findField("in-play")->setText(inPlay ? "YES" : "NO");
}

void CountMetronomeScreen::displayCountIn()
{
	findField("count-in")->setText(countInNames[countIn]);
}

void CountMetronomeScreen::function(int i)
{
	switch (i)
	{
	case 3:
		openScreen("sequencer");
		break;
	case 4:
		openScreen("metronome-sound");
		break;
	}
}

void CountMetronomeScreen::turnWheel(int i)
{
	init();

	if (param == "count-in")
	{
		setCountIn(countIn + i);
	}
	else if (param == "in-play")
	{
		setInPlay(i > 0);
	}
	else if (param == "rate")
	{
		setRate(rate + i);
	}
	else if (param == "in-rec")
	{
		setInRec(i > 0);
	}
	else if (param == "wait-for-key")
	{
		setWaitForKey(i > 0);
	}
}

int CountMetronomeScreen::getCountInMode()
{
	return countIn;
}

void CountMetronomeScreen::setCountIn(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	countIn = i;
	displayCountIn();
}

void CountMetronomeScreen::setInPlay(bool b)
{
	if (inPlay == b)
	{
		return;
	}

	inPlay = b;
	displayInPlay();
}

bool CountMetronomeScreen::getInPlay()
{
	return inPlay;
}

int CountMetronomeScreen::getRate()
{
	return rate;
}

void CountMetronomeScreen::setRate(int i)
{
	if (i < 0 || i > 7)
	{
		return;
	}

	rate = i;
	displayRate();
}

void CountMetronomeScreen::setWaitForKey(bool b)
{
	if (waitForKey == b)
	{
		return;
	}

	waitForKey = b;
	displayWaitForKey();
}

bool CountMetronomeScreen::isWaitForKeyEnabled()
{
	return waitForKey;
}

void CountMetronomeScreen::setInRec(bool b)
{
	if (inRec == b)
	{
		return;
	}

	inRec = b;
	displayInRec();
}

bool CountMetronomeScreen::getInRec()
{
	return inRec;
}
