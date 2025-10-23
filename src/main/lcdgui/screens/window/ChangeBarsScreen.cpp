#include "ChangeBarsScreen.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;

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
	auto seq = sequencer.lock()->getActiveSequence();

	switch (i)
	{
	case 1:
	{
		if (numberOfBars > 0 && afterBar <= seq->getLastBarIndex())
			sequencer.lock()->move(0);

		seq->insertBars(numberOfBars, afterBar);

        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
		break;
	}
	case 4:
	{
		if (firstBar <= seq->getLastBarIndex())
			sequencer.lock()->move(0);

		seq->deleteBars(firstBar, lastBar);
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
		break;
	}
	}
}

void ChangeBarsScreen::turnWheel(int i)
{
	init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "afterbar")
	{
		setAfterBar(afterBar + i);
	}
	else if (focusedFieldName == "numberofbars")
	{
		setNumberOfBars(numberOfBars + i);
	}
	else if (focusedFieldName == "firstbar")
	{
		setFirstBar(firstBar + i);
	}
	else if (focusedFieldName == "lastbar")
	{
		setLastBar(lastBar + i);
	}
}

void ChangeBarsScreen::displayAfterBar()
{
	findField("afterbar")->setTextPadded(afterBar);
}

void ChangeBarsScreen::displayNumberOfBars()
{
	findField("numberofbars")->setTextPadded(numberOfBars);
}

void ChangeBarsScreen::displayFirstBar()
{
	findField("firstbar")->setTextPadded(firstBar + 1);
}

void ChangeBarsScreen::displayLastBar()
{
	findField("lastbar")->setTextPadded(lastBar + 1);
}

void ChangeBarsScreen::setLastBar(int i)
{
	auto seq = sequencer.lock()->getActiveSequence();

	if (i < 0 || i > seq->getLastBarIndex())
		return;

	lastBar = i;

	if (lastBar < firstBar)
		setFirstBar(lastBar);

	displayLastBar();
}

void ChangeBarsScreen::setFirstBar(int i)
{
	auto seq = sequencer.lock()->getActiveSequence();

	if (i < 0 || i > seq->getLastBarIndex())
		return;

	firstBar = i;
	
	displayFirstBar();

	if (firstBar > lastBar)
		setLastBar(firstBar);
}

void ChangeBarsScreen::setNumberOfBars(int i)
{
	auto seq = sequencer.lock()->getActiveSequence();

	if (i < 0 || i > (998 - seq->getLastBarIndex()))
		return;

	numberOfBars = i;
	displayNumberOfBars();
}

void ChangeBarsScreen::setAfterBar(int i)
{
	auto seq = sequencer.lock()->getActiveSequence();

	if (i < 0 || i > seq->getLastBarIndex() + 1)
		return;
	
	afterBar = i;
	displayAfterBar();
}
