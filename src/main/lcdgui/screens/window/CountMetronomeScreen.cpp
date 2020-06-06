#include "CountMetronomeScreen.hpp"

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::window;

CountMetronomeScreen::CountMetronomeScreen(const int layerIndex)
	: ScreenComponent("count-metronome", layerIndex)
{
}

void CountMetronomeScreen::open()
{
	countInField = findField("countin");
	inPlayField = findField("inplay");
	rateField = findField("rate");
	inRecField = findField("inrec");
	waitForKeyField = findField("waitforkey");

	displayCountIn();
	displayInPlay();
	displayRate();
	displayInRec();
	displayWaitForKey();
}

void CountMetronomeScreen::displayWaitForKey()
{
	waitForKeyField.lock()->setText(waitForKey ? "ON" : "OFF");
}

void CountMetronomeScreen::displayInRec()
{
	inRecField.lock()->setText(inRec ? "YES" : "NO");
}

void CountMetronomeScreen::displayRate()
{
	rateField.lock()->setText(rateNames[rate]);
}

void CountMetronomeScreen::displayInPlay()
{
	inPlayField.lock()->setText(inPlay ? "YES" : "NO");
}

void CountMetronomeScreen::displayCountIn()
{
	countInField.lock()->setText(countInNames[countIn]);
}

void CountMetronomeScreen::function(int i)
{
	switch (i) {
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		ls.lock()->openScreen("metronome-sound");
		break;
	}
}

void CountMetronomeScreen::turnWheel(int i)
{
	init();

	if (param.compare("countin") == 0)
	{
		setCountIn(countIn + i);
	}
	else if (param.compare("inplay") == 0)
	{
		setInPlay(i > 0);
	}
	else if (param.compare("rate") == 0)
	{
		setRate(rate + i);
		sequencer.lock()->getActiveSequence().lock()->initMetaTracks();
	}
	else if (param.compare("inrec") == 0)
	{
		setInRec(i > 0);
	}
	else if (param.compare("waitforkey") == 0)
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
		return;

	inRec = b;
	displayInRec();
}

bool CountMetronomeScreen::getInRec()
{
	return inRec;
}
