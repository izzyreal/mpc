#include "CountMetronomeScreen.hpp"

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::window;

CountMetronomeScreen::CountMetronomeScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "count-metronome", layerIndex)
{
}

void CountMetronomeScreen::open()
{
	findField("in-play").lock()->setAlignment(Alignment::Centered);
	findField("in-rec").lock()->setAlignment(Alignment::Centered);
	findField("wait-for-key").lock()->setAlignment(Alignment::Centered);

	displayCountIn();
	displayInPlay();
	displayRate();
	displayInRec();
	displayWaitForKey();
}

void CountMetronomeScreen::displayWaitForKey()
{
	findField("wait-for-key").lock()->setText(waitForKey ? "ON" : "OFF");
}

void CountMetronomeScreen::displayInRec()
{
	findField("in-rec").lock()->setText(inRec ? "YES" : "NO");
}

void CountMetronomeScreen::displayRate()
{
	findField("rate").lock()->setText(rateNames[rate]);
}

void CountMetronomeScreen::displayInPlay()
{
	findField("in-play").lock()->setText(inPlay ? "YES" : "NO");
}

void CountMetronomeScreen::displayCountIn()
{
	findField("count-in").lock()->setText(countInNames[countIn]);
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

	if (param.compare("count-in") == 0)
	{
		setCountIn(countIn + i);
	}
	else if (param.compare("in-play") == 0)
	{
		setInPlay(i > 0);
	}
	else if (param.compare("rate") == 0)
	{
		setRate(rate + i);
		sequencer->getActiveSequence()->initMetaTracks();
	}
	else if (param.compare("in-rec") == 0)
	{
		setInRec(i > 0);
	}
	else if (param.compare("wait-for-key") == 0)
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
