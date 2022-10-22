#include "BarsScreen.hpp"

#include <sequencer/Track.hpp>

#include <lcdgui/screens/EventsScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

BarsScreen::BarsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "bars", layerIndex)
{
}

void BarsScreen::open()
{
	displayFromSq();
	displayToSq();
	displayFirstBar();
	displayLastBar();
	displayAfterBar();
	displayCopies();
}

void BarsScreen::close()
{
	Util::initSequence(mpc);
}

void BarsScreen::function(int j)
{
	init();
	auto eventsScreen = mpc.screens->get<EventsScreen>("events");

	switch (j)
	{
		// Intentional fall-through
	case 0:
	case 2:
	case 3:
		eventsScreen->tab = j;
		openScreen(eventsScreen->tabNames[eventsScreen->tab]);
	break;
	case 4:
		break;
	case 5:
	{
		auto numberOfBars = (lastBar - firstBar + 1) * eventsScreen->copies;

		auto fromSequence = sequencer->getActiveSequence();
		auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();

		if (!toSequence->isUsed())
		{
			toSequence->init(numberOfBars - 1);
		}
		else {
			toSequence->insertBars(numberOfBars, afterBar);
		}

		int copyCounter = 0;

		for (int i = 0; i < numberOfBars; i++)
		{
			toSequence->setTimeSignature(i + afterBar, fromSequence->getNumerator(copyCounter + firstBar), fromSequence->getDenominator(copyCounter + firstBar));
			copyCounter++;
			
			if (copyCounter >= eventsScreen->copies)
				copyCounter = 0;
		}

		auto firstTick = 0;
		auto lastTick = 0;
		auto firstTickOfToSeq = 0;
		auto offset = 0;
		auto segmentLengthTicks = 0;

		for (int i = 0; i < 999; i++)
		{
			if (i == firstBar)
				break;

			firstTick += fromSequence->getBarLengthsInTicks()[i];
		}

		for (int i = 0; i < 999; i++)
		{
			lastTick += fromSequence->getBarLengthsInTicks()[i];
			
			if (i == lastBar)
			{
				break;
			}
		}

		segmentLengthTicks = lastTick - firstTick;

		for (int i = 0; i < 999; i++)
		{
			if (i == afterBar)
				break;

			firstTickOfToSeq += toSequence->getBarLengthsInTicks()[i];
		}

		offset = firstTickOfToSeq - firstTick;

		for (int i = 0; i < 64; i++)
		{
			auto t1 = fromSequence->getTrack(i);
			auto t2 = toSequence->getTrack(i);

			for (auto& e : t1->getEvents())
			{
				auto event = e.lock();

				if (event->getTick() >= firstTick && event->getTick() < lastTick)
				{
					if (!t2->isUsed())
						t2->setUsed(true);

					for (auto k = 0; k < eventsScreen->copies; k++)
					{
						auto clone = t2->cloneEventIntoTrack(event).lock();
						clone->setTick(clone->getTick() + offset + (k * segmentLengthTicks));
					}
				}
			}
		}
		openScreen("sequencer");
		break;
	}
	}
}

void BarsScreen::turnWheel(int i)
{
	init();

	auto eventsScreen = mpc.screens->get<EventsScreen>("events");
	auto fromSequence = sequencer->getActiveSequence();
	auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();

	if (param.compare("fromsq") == 0)
	{
		sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() + i);

		displayFromSq();

		auto lastBarIndex = fromSequence->getLastBarIndex();

		if (lastBar > lastBarIndex)
			setLastBar(lastBarIndex, lastBarIndex); // Implies any necessary calls to setFirstBar
	}
	else if (param.compare("tosq") == 0)
	{
		eventsScreen->setToSq(eventsScreen->toSq + i);

		displayToSq();

		if (afterBar > toSequence->getLastBarIndex())
			setAfterBar(toSequence->getLastBarIndex(), toSequence->getLastBarIndex());
	}
	else if (param.compare("afterbar") == 0)
	{
		setAfterBar(afterBar + i, toSequence->getLastBarIndex());
	}
	else if (param.compare("firstbar") == 0)
	{
		setFirstBar(firstBar + i, fromSequence->getLastBarIndex());
	}
	else if (param.compare("lastbar") == 0)
	{
		setLastBar(lastBar + i, fromSequence->getLastBarIndex());
	}
	else if (param.compare("copies") == 0)
	{
		setCopies(eventsScreen->copies + i);
	}
}

void BarsScreen::displayCopies()
{
	auto eventsScreen = mpc.screens->get<EventsScreen>("events");
	findField("copies").lock()->setTextPadded(eventsScreen->copies, " ");
}

void BarsScreen::displayToSq()
{
	auto eventsScreen = mpc.screens->get<EventsScreen>("events");
	findField("tosq").lock()->setText(to_string(eventsScreen->toSq + 1));
}

void BarsScreen::displayFromSq()
{
	findField("fromsq").lock()->setText(to_string(sequencer->getActiveSequenceIndex() + 1));
}

void BarsScreen::displayAfterBar()
{
	findField("afterbar").lock()->setText(to_string(afterBar));
}

void BarsScreen::displayLastBar()
{
	findField("lastbar").lock()->setText(to_string(lastBar + 1));
}

void BarsScreen::displayFirstBar()
{
	findField("firstbar").lock()->setText(to_string(firstBar + 1));
}

void BarsScreen::setLastBar(int i, int max)
{
	if (i < 0 || i > max)
		return;

	lastBar = i;

	if (lastBar < firstBar)
		setFirstBar(lastBar, max);

	displayLastBar();
}

void BarsScreen::setFirstBar(int i, int max)
{
	if (i < 0 || i > max)
		return;

	firstBar = i;
	
	if (firstBar > lastBar)
		setLastBar(firstBar, max);

	displayFirstBar();
}

void BarsScreen::setAfterBar(int i, int max)
{
	if (i < 0 || i > max + 1)
		return;

	afterBar = i;
	displayAfterBar();
}

void BarsScreen::setCopies(int i)
{
	if (i < 1 || i > 999)
		return;

	auto eventsScreen = mpc.screens->get<EventsScreen>("events");
	eventsScreen->copies = i;
	displayCopies();
}
