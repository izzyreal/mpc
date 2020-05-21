#include "ChangeBarsScreen.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeBarsScreen::ChangeBarsScreen(const int& layer)
	: ScreenComponent("changebars", layer)
{
}

void ChangeBarsScreen::open()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock();

	if (getAfterBar() > sequence->getLastBar() + 1)
	{
		setAfterBar(sequence->getLastBar() + 1, sequence->getLastBar());
	}

	if (getFirstBar() > sequence->getLastBar())
	{
		setFirstBar(sequence->getLastBar(), sequence->getLastBar());
	}

	if (getLastBar() > sequence->getLastBar())
	{
		setLastBar(sequence->getLastBar(), sequence->getLastBar());
	}

	displayAfterBar();
	displayNumberOfBars();
	displayFirstBar();
	displayLastBar();
}

void ChangeBarsScreen::function(int i)
{
    BaseControls::function(i);
	auto seq = sequencer.lock()->getActiveSequence().lock();

	switch (i)
	{
	case 1:
		seq->insertBars(getNumberOfBars(), getAfterBar());
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		seq->deleteBars(getFirstBar(), getLastBar());
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void ChangeBarsScreen::turnWheel(int i)
{
	init();
	auto seq = sequencer.lock()->getActiveSequence().lock();

	if (param.compare("afterbar") == 0)
	{
		setAfterBar(getAfterBar() + i, seq->getLastBar());
	}
	else if (param.compare("numberofbars") == 0)
	{
		setNumberOfBars(getNumberOfBars() + i, seq->getLastBar());
	}
	else if (param.compare("firstbar") == 0)
	{
		setFirstBar(getFirstBar() + i, seq->getLastBar());
	}
	else if (param.compare("lastbar") == 0)
	{
		setLastBar(getLastBar() + i, seq->getLastBar());
	}
}


void ChangeBarsScreen::displayFirstBar()
{
	findField("firstbar").lock()->setText(to_string(getFirstBar() + 1));
}

void ChangeBarsScreen::displayLastBar()
{
	findField("lastbar").lock()->setText(to_string(getLastBar() + 1));
}

void ChangeBarsScreen::displayNumberOfBars()
{
	findField("numberofbars").lock()->setText(to_string(getNumberOfBars()));
}

void ChangeBarsScreen::displayAfterBar()
{
	findField("afterbar").lock()->setText(to_string(getAfterBar()));
}

int ChangeBarsScreen::getLastBar()
{
	return lastBar;
}

void ChangeBarsScreen::setLastBar(int i, int max)
{
	if (i < 0 || i > max)
	{
		return;
	}

	lastBar = i;

	if (lastBar < firstBar)
	{
		setFirstBar(lastBar, max);
	}

	displayLastBar();
}

int ChangeBarsScreen::getFirstBar()
{
	return firstBar;
}

void ChangeBarsScreen::setFirstBar(int i, int max)
{
	if (i < 0 || i > max)
	{
		return;
	}

	firstBar = i;
	
	displayFirstBar();

	if (firstBar > lastBar)
	{
		setLastBar(firstBar, max);
	}
}

int ChangeBarsScreen::getNumberOfBars()
{
	return numberOfBars;
}

void ChangeBarsScreen::setNumberOfBars(int i, int max)
{
	if (i < 0 || i >(999 - (max + 1)))
	{
		return;
	}

	numberOfBars = i;
	displayNumberOfBars();
}

int ChangeBarsScreen::getAfterBar()
{
	return afterBar;
}

void ChangeBarsScreen::setAfterBar(int i, int max)
{
	if (i < 0 || i > max + 1)
	{
		return;
	}
	
	afterBar = i;
	displayAfterBar();
}
