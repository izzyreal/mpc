#include "BarsScreen.hpp"

#include <sequencer/Track.hpp>

#include <lcdgui/screens/EventsScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;

BarsScreen::BarsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "bars", layerIndex)
{
}

void BarsScreen::open()
{
    auto fromSequence = sequencer->getActiveSequence();
    auto eventsScreen = mpc.screens->get<EventsScreen>("events");
    auto toSequence = sequencer->getSequence(eventsScreen->toSq);
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
        copyBars(eventsScreen->toSq, firstBar, lastBar, eventsScreen->copies, afterBar);
        sequencer->setActiveSequenceIndex(eventsScreen->toSq);
		openScreen("sequencer");
		break;
	}
	}
}

void BarsScreen::copyBars(int toSeqIndex, int copyFirstBar, int copyLastBar, int copyCount, int copyAfterBar)
{
    auto fromSequence = sequencer->getActiveSequence();

    if (!fromSequence->isUsed())
    {
        return;
    }

    auto eventsScreen = mpc.screens->get<EventsScreen>("events");

    auto toSequence = sequencer->getSequence(toSeqIndex);
    auto numberOfBars = (copyLastBar - copyFirstBar + 1) * copyCount;

    if (numberOfBars > 999)
    {
        numberOfBars = 999;
    }

    if (!toSequence->isUsed())
    {
        toSequence->init(numberOfBars - 1);
    }
    else
    {
        if (toSequence->getLastBarIndex() + numberOfBars > 998)
        {
            numberOfBars = 998 - toSequence->getLastBarIndex();
        }

        toSequence->insertBars(numberOfBars, copyAfterBar);
    }

    int copyCounter = 0;

    for (int i = 0; i < numberOfBars; i++)
    {
        toSequence->setTimeSignature(i + copyAfterBar, fromSequence->getNumerator(copyCounter + copyFirstBar), fromSequence->getDenominator(copyCounter + copyFirstBar));
        copyCounter++;

        if (copyCounter >= copyCount)
            copyCounter = 0;
    }

    auto firstTick = 0;
    auto lastTick = 0;
    auto firstTickOfToSeq = 0;

    for (int i = 0; i < 999; i++)
    {
        if (i == copyFirstBar)
            break;

        firstTick += fromSequence->getBarLengthsInTicks()[i];
    }

    for (int i = 0; i < 999; i++)
    {
        lastTick += fromSequence->getBarLengthsInTicks()[i];

        if (i == copyLastBar)
        {
            break;
        }
    }

    for (int i = 0; i < 999; i++)
    {
        if (i == copyAfterBar)
            break;

        firstTickOfToSeq += toSequence->getBarLengthsInTicks()[i];
    }

    auto segmentLengthTicks = lastTick - firstTick;
    auto offset = firstTickOfToSeq - firstTick;

    for (int i = 0; i < 64; i++)
    {
        auto t1 = fromSequence->getTrack(i);

        if (!t1->isUsed())
        {
            continue;
        }

        auto t1Events = t1->getEventRange(firstTick, lastTick);
        auto t2 = toSequence->getTrack(i);

        if (!t2->isUsed())
        {
            t2->setUsed(true);
        }

        auto toSeqLastTick = toSequence->getLastTick();

        for (auto& event : t1Events)
        {
            auto firstCopyTick = event->getTick() + offset;

            if (firstCopyTick >= toSeqLastTick)
            {
                // The events in Track are ordered by tick, so this and
                // any following event copies would be out of bounds.
                break;
            }

            for (auto k = 0; k < copyCount; k++)
            {
                auto tick = firstCopyTick + (k * segmentLengthTicks);

                // We do a more specific exit-check here, since within-bounds
                // copies may be followed by out-of-bounds ones.
                if (tick >= toSeqLastTick)
                {
                    break;
                }

                t2->cloneEventIntoTrack(event, tick);
            }
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

        auto toSequence = sequencer->getSequence(eventsScreen->toSq);
        auto maxAfterBarIndex = toSequence->isUsed() ? toSequence->getLastBarIndex() + 1 : 0;

        if (afterBar > maxAfterBarIndex)
        {
            setAfterBar(maxAfterBarIndex, maxAfterBarIndex);
        }
	}
	else if (param == "afterbar")
	{
        auto toSequence = sequencer->getSequence(eventsScreen->toSq);

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
        auto toSequence = sequencer->getSequence(eventsScreen->toSq);

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
	findField("tosq").lock()->setText(std::to_string(eventsScreen->toSq + 1));
}

void BarsScreen::displayFromSq()
{
	findField("fromsq").lock()->setText(std::to_string(sequencer->getActiveSequenceIndex() + 1));
}

void BarsScreen::displayAfterBar()
{
	findField("afterbar").lock()->setText(std::to_string(afterBar));
}

void BarsScreen::displayLastBar()
{
	findField("lastbar").lock()->setText(std::to_string(lastBar + 1));
}

void BarsScreen::displayFirstBar()
{
	findField("firstbar").lock()->setText(std::to_string(firstBar + 1));
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
