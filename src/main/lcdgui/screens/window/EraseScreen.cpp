#include "EraseScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <Util.hpp>
#include <System.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace moduru;
using namespace std;

EraseScreen::EraseScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "erase", layerIndex)
{
}

void EraseScreen::open()
{
	auto bus = sequencer.lock()->getActiveTrack().lock()->getBus();

	if (bus == 0)
	{
		findField("note0").lock()->setAlignment(Alignment::Centered, 18);
		findField("note1").lock()->setAlignment(Alignment::Centered, 18);
		findField("note0").lock()->setLocation(62, 42);
	}
	else
	{
		findField("note0").lock()->setAlignment(Alignment::None);
		findField("note1").lock()->setAlignment(Alignment::None);
		findField("note0").lock()->setLocation(61, 42);
	}

	findField("note1").lock()->setLocation(116, 42);

	setTime0(0);

	auto seq = sequencer.lock()->getActiveSequence().lock();
	setTime1(seq->getLastTick());
	
	displayErase();
	displayNotes();
	displayTime();
	displayTrack();
	displayType();
}

void EraseScreen::turnWheel(int i)
{
	init();

	if (checkAllTimesAndNotes(mpc, i))
		return;

	if (param == "track")
	{
		setTrack(track + i);
	}
	else if (param == "erase")
	{
		setErase(erase + i);
	}
	else if (param == "type")
	{
		setType(type + i);
	}
}

void EraseScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		openScreen("sequencer");
		break;
	case 4:
	{
		auto startIndex = track - 1;
		auto lastIndex = track - 1;

		if (startIndex < 0)
		{
			startIndex = 0;
			lastIndex = 63;
		}

		auto midi = sequencer.lock()->getActiveTrack().lock()->getBus() == 0;

		auto noteA = note0;
		auto noteB = midi ? note1 : -1;

		auto seq = sequencer.lock()->getActiveSequence().lock();

		for (auto j = startIndex; j < lastIndex + 1; j++)
		{
			vector<int> removalIndices;
			auto t = seq->getTrack(j).lock();
			
			for (auto k = 0; k < t->getEvents().size(); k++)
			{
				auto e = t->getEvent(k).lock();
				auto ne = dynamic_pointer_cast<NoteEvent>(e);

				if (e->getTick() >= time0 && e->getTick() < time1)
				{
					string excludeClass = "";
					string includeClass = "";
					
					switch (erase)
					{
					case 0:
						if (ne)
						{
							auto nn = ne->getNote();
							
							if (midi && nn >= noteA && nn <= noteB)
								removalIndices.push_back(k);
							
							if (!midi && (noteA <= 34 || noteA == nn))
								removalIndices.push_back(k);
						}
						else
						{
							removalIndices.push_back(k);
						}
						break;
					case 1:
						excludeClass = eventClassNames[type];

						if (System::demangle(typeid(e).name()) != excludeClass)
						{
							if (ne)
							{
								auto nn = ne->getNote();
							
								if (midi && nn >= noteA && nn <= noteB)
									removalIndices.push_back(k);
								
								if (!midi && (noteA > 34 && noteA != nn))
									removalIndices.push_back(k);
							}
							else
							{
								removalIndices.push_back(k);
							}
						}
						break;
					case 2:
						includeClass = eventClassNames[type];
						
						if (System::demangle(typeid(e).name()).compare(includeClass) == 0)
						{
							if (ne)
							{
								auto nn = ne->getNote();
							
								if (midi && nn >= noteA && nn <= noteB)
									removalIndices.push_back(k);
								
								if (!midi && (noteA <= 34 || noteA == nn))
									removalIndices.push_back(k);
							}
							else
							{
								removalIndices.push_back(k);
							}
						}
						break;
					}

				}
			}

			sort(begin(removalIndices), end(removalIndices));
			reverse(begin(removalIndices), end(removalIndices));

			for (int index : removalIndices)
            {
                t->removeEvent(index);
            }
		}
		openScreen("sequencer");
		break;
	}
	}
}

void EraseScreen::displayTrack()
{
	string trackName = "";

	if (track == -1)
	{
		trackName = "ALL";
	}
	else
	{
		auto sequence = sequencer.lock()->getActiveSequence().lock();
		trackName = sequence->getTrack(track).lock()->getActualName();
	}

	findField("track").lock()->setTextPadded(track + 1, " ");
	findLabel("track-name").lock()->setText("-" + trackName);
}

void EraseScreen::displayTime()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock().get();
	findField("time0").lock()->setTextPadded(SeqUtil::getBarFromTick(sequence, time0) + 1, "0");
	findField("time1").lock()->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
	findField("time2").lock()->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
	findField("time3").lock()->setTextPadded(SeqUtil::getBarFromTick(sequence, time1) + 1, "0");
	findField("time4").lock()->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
	findField("time5").lock()->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void EraseScreen::displayErase()
{
	findField("erase").lock()->setText(eraseNames[erase]);
}

void EraseScreen::displayType()
{
	findField("type").lock()->Hide(erase == 0);

	if (erase > 0)
		findField("type").lock()->setText(typeNames[type]);
}

void EraseScreen::displayNotes()
{	
	init();

	if (erase != 0 && ((erase == 1 && type != 0) || (erase == 2 && type != 0)))
	{
		findField("note0").lock()->Hide(true);
		findLabel("note0").lock()->Hide(true);
		findField("note1").lock()->Hide(true);
		findLabel("note1").lock()->Hide(true);
		return;
	}

	auto bus = sequencer.lock()->getActiveTrack().lock()->getBus();

	findField("note0").lock()->Hide(false);
	findLabel("note0").lock()->Hide(false);
	findField("note1").lock()->Hide(bus != 0);
	findLabel("note1").lock()->Hide(bus != 0);

	if (bus == 0)
	{
		findField("note0").lock()->setSize(47, 9);
		findField("note0").lock()->setText((StrUtil::padLeft(to_string(note0), " ", 3) + "(" + mpc::Util::noteNames()[note0]) + ")");
		findField("note1").lock()->setText((StrUtil::padLeft(to_string(note1), " ", 3) + "(" + mpc::Util::noteNames()[note1]) + ")");
	}
	else
	{
		findField("note0").lock()->setSize(37, 9);

		if (note0 == 34)
			findField("note0").lock()->setText("ALL");
		else
        {
            auto padIndexWithBank = program.lock()->getPadIndexFromNote(note0);
            auto padName = sampler->getPadName(padIndexWithBank);
            findField("note0").lock()->setText(to_string(note0) + "/" + padName);
        }
	}
}

void EraseScreen::setTrack(int i)
{
	if (i < -1 || i > 63)
		return;

	track = i;
	displayTrack();
}

void EraseScreen::setErase(int i)
{
	if (i < 0 || i > 2)
		return;

	erase = i;
	displayErase();
	displayType();
	displayNotes();
}

void EraseScreen::setType(int i)
{
	if (i < 0 || i > 6)
		return;

	type = i;
	displayType();
	displayNotes();
}
