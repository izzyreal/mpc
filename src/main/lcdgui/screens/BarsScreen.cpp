#include "BarsScreen.hpp"

#include <sequencer/Track.hpp>

#include <lcdgui/screens/EventsScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

BarsScreen::BarsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "bars", layerIndex)
{
}

void BarsScreen::open()
{
    auto fromSequence = sequencer->getActiveSequence();
    auto eventsScreen = mpc.screens->get<EventsScreen>("events");
    auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();
    auto userScreen = mpc.screens->get<UserScreen>("user");
    auto userLastBar = userScreen->lastBar;

    auto lastBarIndexFrom = fromSequence->isUsed() ? fromSequence->getLastBarIndex() : userLastBar;
    auto maxAfterBarIndex = toSequence->isUsed() ? toSequence->getLastBarIndex() + 1 : 0;

    if (firstBar > lastBarIndexFrom)
    {
        setFirstBar(lastBarIndexFrom, lastBarIndexFrom);
    }

    if (lastBar > lastBarIndexFrom)
    {
        setLastBar(lastBarIndexFrom, lastBarIndexFrom);
    }

    if (afterBar > maxAfterBarIndex)
    {
        setAfterBar(maxAfterBarIndex, maxAfterBarIndex);
    }

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
		auto fromSequence = sequencer->getActiveSequence();

        if (!fromSequence->isUsed())
        {
            return;
        }

		auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();
        auto numberOfBars = (lastBar - firstBar + 1) * eventsScreen->copies;

		if (!toSequence->isUsed())
		{
			toSequence->init(numberOfBars - 1);
		}
		else
        {
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

        auto segmentLengthTicks = lastTick - firstTick;

		for (int i = 0; i < 999; i++)
		{
			if (i == afterBar)
				break;

			firstTickOfToSeq += toSequence->getBarLengthsInTicks()[i];
		}

		auto offset = firstTickOfToSeq - firstTick;

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
        sequencer->setActiveSequenceIndex(eventsScreen->toSq);
		openScreen("sequencer");
		break;
	}
	}
}

void BarsScreen::turnWheel(int i)
{
	init();

	auto eventsScreen = mpc.screens->get<EventsScreen>("events");
    auto userScreen = mpc.screens->get<UserScreen>("user");
    auto userLastBar = userScreen->lastBar;

	if (param == "fromsq")
	{
		sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() + i);

		displayFromSq();

    	auto fromSequence = sequencer->getActiveSequence();
		auto lastBarIndex = fromSequence->isUsed() ? fromSequence->getLastBarIndex() : userLastBar;

        if (firstBar > lastBarIndex)
        {
            setFirstBar(lastBarIndex, lastBarIndex);
        }

		if (lastBar > lastBarIndex)
        {
            setLastBar(lastBarIndex, lastBarIndex);
        }
	}
	else if (param == "tosq")
	{
		eventsScreen->setToSq(eventsScreen->toSq + i);

		displayToSq();

        auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();
        auto maxAfterBarIndex = toSequence->isUsed() ? toSequence->getLastBarIndex() + 1 : 0;

        if (afterBar > maxAfterBarIndex)
        {
            setAfterBar(maxAfterBarIndex, maxAfterBarIndex);
        }
	}
	else if (param == "afterbar")
	{
        auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();

        if (!toSequence->isUsed())
        {
            return;
        }

		setAfterBar(afterBar + i, toSequence->getLastBarIndex() + 1);
	}
	else if (param == "firstbar")
	{
        auto fromSequence = sequencer->getActiveSequence();
        auto lastBarIndex = fromSequence->isUsed() ? fromSequence->getLastBarIndex() : userLastBar;

        setFirstBar(firstBar + i, lastBarIndex);
	}
	else if (param == "lastbar")
	{
        auto fromSequence = sequencer->getActiveSequence();
        auto lastBarIndex = fromSequence->isUsed() ? fromSequence->getLastBarIndex() : userLastBar;

        setLastBar(lastBar + i, lastBarIndex);
	}
	else if (param == "copies")
	{
        auto toSequence = sequencer->getSequence(eventsScreen->toSq).lock();

        if (!toSequence->isUsed())
        {
            mpc::Util::initSequence(eventsScreen->toSq, mpc);
        }

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
	if (i < 0)
    {
        return;
    }

    if (i > max)
    {
        if (lastBar == max)
        {
            return;
        }
        i = max;
    }

	lastBar = i;

	if (lastBar < firstBar)
		setFirstBar(lastBar, max);

	displayLastBar();
}

void BarsScreen::setFirstBar(int i, int max)
{
	if (i < 0)
    {
        return;
    }
    if (i > max)
    {
        if (firstBar == max)
        {
            return;
        }
        i = max;
    }

	firstBar = i;
	
	if (firstBar > lastBar)
		setLastBar(firstBar, max);

	displayFirstBar();
}

void BarsScreen::setAfterBar(int i, int max)
{
	if (i < 0)
    {
        return;
    }

    if (i > max)
    {
        if (afterBar == max)
        {
            return;
        }
        i = max;
    }

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
