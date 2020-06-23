#include "BarCopyScreen.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>

#include <lcdgui/screens/EventsScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

BarCopyScreen::BarCopyScreen(const int layerIndex)
	: ScreenComponent("bar-copy", layerIndex)
{
}

void BarCopyScreen::open()
{
	displayFromSq();
	displayToSq();
	displayFirstBar();
	displayLastBar();
	displayAfterBar();
	displayCopies();
}

void BarCopyScreen::function(int j)
{
	init();

	switch (j)
	{
	case 0:
	{
		ls.lock()->openScreen("events");
		break;
	}
	case 2:
		ls.lock()->openScreen("tr-move");
		break;
	case 3:
		ls.lock()->openScreen("user");
		break;
	case 4:
		break;
	case 5:
	{
		auto numberOfBars = (lastBar - firstBar + 1) * copies;

		auto eventsScreen = dynamic_pointer_cast<EventsScreen>(Screens::getScreenComponent("events"));
		auto fromSequence = sequencer.lock()->getSequence(eventsScreen->fromSq).lock();
		auto toSequence = sequencer.lock()->getSequence(eventsScreen->toSq).lock();

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
			
			if (copyCounter >= copies)
			{
				copyCounter = 0;
			}
		}

		auto firstTick = 0;
		auto lastTick = 0;
		auto firstTickOfToSeq = 0;
		auto offset = 0;
		auto segmentLengthTicks = 0;

		for (int i = 0; i < 999; i++)
		{
			if (i == firstBar)
			{
				break;
			}
			firstTick += (*fromSequence->getBarLengths())[i];
		}

		for (int i = 0; i < 999; i++)
		{
			lastTick += (*fromSequence->getBarLengths())[i];
			
			if (i == lastBar)
			{
				break;
			}
		}

		segmentLengthTicks = lastTick - firstTick;

		for (int i = 0; i < 999; i++)
		{
			if (i == afterBar)
			{
				break;
			}

			firstTickOfToSeq += (*toSequence->getBarLengths())[i];
		}

		offset = firstTickOfToSeq - firstTick;

		for (int i = 0; i < 64; i++)
		{
			auto t1 = fromSequence->getTrack(i).lock();
			auto t2 = toSequence->getTrack(i).lock();

			for (auto& e : t1->getEvents())
			{
				auto event = e.lock();

				if (event->getTick() >= firstTick && event->getTick() < lastTick)
				{
					if (!t2->isUsed())
					{
						t2->setUsed(true);
					}

					for (auto k = 0; k < copies; k++)
					{
						auto clone = t2->cloneEvent(event).lock();
						clone->setTick(clone->getTick() + offset + (k * segmentLengthTicks));
					}
				}
			}
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}

void BarCopyScreen::turnWheel(int i)
{
	init();

	auto eventsScreen = dynamic_pointer_cast<EventsScreen>(Screens::getScreenComponent("events"));
	auto fromSequence = sequencer.lock()->getSequence(eventsScreen->fromSq).lock();
	auto toSequence = sequencer.lock()->getSequence(eventsScreen->toSq).lock();

	if (param.compare("fromsq") == 0)
	{

		eventsScreen->fromSq += i;

		displayFromSq();

		if (lastBar > fromSequence->getLastBar())
		{
			setLastBar(fromSequence->getLastBar(), fromSequence->getLastBar());
		}

	}
	else if (param.compare("tosq") == 0)
	{
		eventsScreen->toSq += i;

		displayToSq();

		if (afterBar > toSequence->getLastBar())
		{
			setAfterBar(toSequence->getLastBar(), toSequence->getLastBar());
		}

	}
	else if (param.compare("afterbar") == 0)
	{
		setAfterBar(afterBar + i, toSequence->getLastBar());
	}
	else if (param.compare("firstbar") == 0)
	{
		setFirstBar(firstBar + i, fromSequence->getLastBar());
	}
	else if (param.compare("lastbar") == 0)
	{
		setLastBar(lastBar + i, fromSequence->getLastBar());
	}
	else if (param.compare("copies") == 0)
	{
		setCopies(copies + i);
	}
}

void BarCopyScreen::displayCopies()
{
	findField("copies").lock()->setTextPadded(copies, " ");
}

void BarCopyScreen::displayToSq()
{
	auto eventsScreen = dynamic_pointer_cast<EventsScreen>(Screens::getScreenComponent("events"));
	findField("tosq").lock()->setText(to_string(eventsScreen->toSq + 1));
}

void BarCopyScreen::displayFromSq()
{
	auto eventsScreen = dynamic_pointer_cast<EventsScreen>(Screens::getScreenComponent("events"));
	findField("fromsq").lock()->setText(to_string(eventsScreen->fromSq + 1));
}

void BarCopyScreen::displayAfterBar()
{
	findField("afterbar").lock()->setText(to_string(afterBar));
}

void BarCopyScreen::displayLastBar()
{
	findField("lastbar").lock()->setText(to_string(lastBar + 1));
}

void BarCopyScreen::displayFirstBar()
{
	findField("firstbar").lock()->setText(to_string(firstBar + 1));
}

void BarCopyScreen::setLastBar(int i, int max)
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

void BarCopyScreen::setFirstBar(int i, int max)
{
	if (i < 0 || i > max)
	{
		return;
	}

	firstBar = i;
	
	if (firstBar > lastBar)
	{
		setLastBar(firstBar, max);
	}
	displayFirstBar();
}

void BarCopyScreen::setAfterBar(int i, int max)
{
	if (i < 0 || i > max + 1)
	{
		return;
	}

	afterBar = i;
	displayAfterBar();
}

void BarCopyScreen::setCopies(int i)
{
	if (i < 1 || i > 999)
	{
		return;
	}

	copies = i;
	displayCopies();
}
