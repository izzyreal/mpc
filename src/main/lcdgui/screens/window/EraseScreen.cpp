#include "EraseScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

EraseScreen::EraseScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "erase", layerIndex)
{
}

void EraseScreen::open()
{
	auto bus = sequencer.lock()->getActiveTrack()->getBus();

	if (bus == 0)
	{
		findField("note0")->setAlignment(Alignment::Centered, 18);
		findField("note1")->setAlignment(Alignment::Centered, 18);
		findField("note0")->setLocation(62, 42);
	}
	else
	{
		findField("note0")->setAlignment(Alignment::None);
		findField("note1")->setAlignment(Alignment::None);
		findField("note0")->setLocation(61, 42);
	}

	findField("note1")->setLocation(116, 42);

	setTime0(0);

	auto seq = sequencer.lock()->getActiveSequence();
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
        doErase();
		openScreen("sequencer");
		break;
	}
}

void EraseScreen::displayTrack()
{
    std::string trackName;

	if (track == -1)
	{
		trackName = "ALL";
	}
	else
	{
		auto sequence = sequencer.lock()->getActiveSequence();
		trackName = sequence->getTrack(track)->getActualName();
	}

	findField("track")->setTextPadded(track + 1, " ");
	findLabel("track-name")->setText("-" + trackName);
}

void EraseScreen::displayTime()
{
	auto sequence = sequencer.lock()->getActiveSequence().get();
	findField("time0")->setTextPadded(SeqUtil::getBarFromTick(sequence, time0) + 1, "0");
	findField("time1")->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
	findField("time2")->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
	findField("time3")->setTextPadded(SeqUtil::getBarFromTick(sequence, time1) + 1, "0");
	findField("time4")->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
	findField("time5")->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void EraseScreen::displayErase()
{
	findField("erase")->setText(eraseNames[erase]);
}

void EraseScreen::displayType()
{
	findField("type")->Hide(erase == 0);

	if (erase > 0)
		findField("type")->setText(typeNames[type]);
}

void EraseScreen::displayNotes()
{	
	init();

	if (erase != 0 && ((erase == 1 && type != 0) || (erase == 2 && type != 0)))
	{
		findField("note0")->Hide(true);
		findLabel("note0")->Hide(true);
		findField("note1")->Hide(true);
		findLabel("note1")->Hide(true);
		return;
	}

	auto bus = sequencer.lock()->getActiveTrack()->getBus();

	findField("note0")->Hide(false);
	findLabel("note0")->Hide(false);
	findField("note1")->Hide(bus != 0);
	findLabel("note1")->Hide(bus != 0);

	if (bus == 0)
	{
		findField("note0")->setSize(47, 9);
		findField("note0")->setText((StrUtil::padLeft(std::to_string(note0), " ", 3) + "(" + mpc::Util::noteNames()[note0]) + ")");
		findField("note1")->setText((StrUtil::padLeft(std::to_string(note1), " ", 3) + "(" + mpc::Util::noteNames()[note1]) + ")");
	}
	else
	{
		findField("note0")->setSize(37, 9);

		if (note0 == 34)
			findField("note0")->setText("ALL");
		else
        {
            auto padIndexWithBank = program->getPadIndexFromNote(note0);
            auto padName = sampler->getPadName(padIndexWithBank);
            findField("note0")->setText(std::to_string(note0) + "/" + padName);
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

void EraseScreen::doErase()
{
    const auto firstTrackIndex = track < 0 ? 0 : track;
    const auto lastTrackIndex = track < 0 ? 63 : track;

    const auto midi = sequencer.lock()->getActiveTrack()->getBus() == 0;

    const auto noteA = note0;
    const auto noteB = midi ? note1 : -1;

    auto seq = sequencer.lock()->getActiveSequence();

    const auto selectedType = eventTypes[type];

    for (auto trackIndex = firstTrackIndex; trackIndex <= lastTrackIndex; trackIndex++)
    {
        const auto seqTrack = seq->getTrack(trackIndex);

        for (auto eventIndex = static_cast<int>(seqTrack->getEvents().size()) - 1; eventIndex >= 0; eventIndex--)
        {
            const auto event = seqTrack->getEvent(eventIndex);
            const auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);

            if (event->getTick() >= time0 && event->getTick() < time1)
            {
                if (erase == 0 ||
                    (erase == 1 && event->getTypeName() != selectedType) ||
                    erase == 2 && event->getTypeName() != selectedType)
                {
                    if (noteEvent)
                    {
                        const auto nn = noteEvent->getNote();
                        if ((midi && nn >= noteA && nn <= noteB) || (!midi && (noteA <= 34 || noteA == nn)))
                        {
                            seqTrack->removeEvent(eventIndex);
                        }
                    }
                    else
                    {
                        seqTrack->removeEvent(eventIndex);
                    }
                }
            }
        }
    }
}