#include "ChangeBarsScreen.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeBarsScreen::ChangeBarsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "change-bars", layerIndex)
{
}

void ChangeBarsScreen::open()
{
	setAfterBar(0);
	setNumberOfBars(0);
	setFirstBar(0);
	setLastBar(0);
}

void ChangeBarsScreen::function(int i)
{
    ScreenComponent::function(i);
	auto seq = sequencer->getActiveSequence().lock();

	switch (i)
	{
	case 1:
	{
		if (numberOfBars > 0 && afterBar <= seq->getLastBarIndex())
			sequencer->move(0);

		seq->insertBars(numberOfBars, afterBar);

		openScreen("sequencer");
		break;
	}
	case 4:
	{
		if (firstBar <= seq->getLastBarIndex())
			sequencer->move(0);

		seq->deleteBars(firstBar, lastBar);
		openScreen("sequencer");
		break;
	}
	}
}

void ChangeBarsScreen::turnWheel(int i)
{
	init();

	if (param.compare("afterbar") == 0)
	{
		setAfterBar(afterBar + i);
	}
	else if (param.compare("numberofbars") == 0)
	{
		setNumberOfBars(numberOfBars + i);
	}
	else if (param.compare("firstbar") == 0)
	{
		setFirstBar(firstBar + i);
	}
	else if (param.compare("lastbar") == 0)
	{
		setLastBar(lastBar + i);
	}
}

void ChangeBarsScreen::displayAfterBar()
{
	findField("afterbar").lock()->setTextPadded(afterBar);
}

void ChangeBarsScreen::displayNumberOfBars()
{
	findField("numberofbars").lock()->setTextPadded(numberOfBars);
}

void ChangeBarsScreen::displayFirstBar()
{
	findField("firstbar").lock()->setTextPadded(firstBar + 1);
}

void ChangeBarsScreen::displayLastBar()
{
	findField("lastbar").lock()->setTextPadded(lastBar + 1);
}

void ChangeBarsScreen::setLastBar(int i)
{
	auto seq = sequencer->getActiveSequence().lock();

	if (i < 0 || i > seq->getLastBarIndex())
		return;

	lastBar = i;

	if (lastBar < firstBar)
		setFirstBar(lastBar);

	displayLastBar();
}

void ChangeBarsScreen::setFirstBar(int i)
{
	auto seq = sequencer->getActiveSequence().lock();

	if (i < 0 || i > seq->getLastBarIndex())
		return;

	firstBar = i;
	
	displayFirstBar();

	if (firstBar > lastBar)
		setLastBar(firstBar);
}

void ChangeBarsScreen::setNumberOfBars(int i)
{
	auto seq = sequencer->getActiveSequence().lock();

	if (i < 0 || i > (998 - seq->getLastBarIndex()))
		return;

	numberOfBars = i;
	displayNumberOfBars();
}

void ChangeBarsScreen::setAfterBar(int i)
{
	auto seq = sequencer->getActiveSequence().lock();

	if (i < 0 || i > seq->getLastBarIndex() + 1)
		return;
	
	afterBar = i;
	displayAfterBar();
}
