#include "EventsScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lcdgui/screens/UserScreen.hpp>

#include <Util.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

EventsScreen::EventsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "events", layerIndex)
{
}

void EventsScreen::pad(int padIndexWithBank, int velo)
{
    ScreenComponent::pad(padIndexWithBank, velo);

    init();

    if (param != "note0")
    {
        return;
    }

    int newNote = program.lock()->getNoteFromPad(padIndexWithBank);

    if (newNote >= 35)
    {
        note0 = newNote;
        displayNotes();
    }
}

void EventsScreen::open()
{
	sequencer->move(0);

	auto note1Field = findField("note1").lock();

	if (setNote1X)
	{
		note1Field->setLocation(note1Field->getX() + 1, note1Field->getY());
		setNote1X = false;
	}

	note1Field->setSize(47, 9);
	note1Field->setAlignment(Alignment::Centered, 18);

    setFromSq(sequencer->getActiveSequenceIndex());
    setToSq(sequencer->getActiveSequenceIndex());

	if (tab != 0)
	{
		openScreen(tabNames[tab]);
		return;
	}

	setFromTr(sequencer->getActiveTrackIndex());
	setToTr(sequencer->getActiveTrackIndex());

	auto seq = sequencer->getActiveSequence();

	if (!seq->isUsed())
	{
		auto userScreen = mpc.screens->get<UserScreen>("user");
		seq->init(userScreen->lastBar);
	}

	time0 = 0;
	time1 = seq->getLastTick();
	displayTime();

	displayEdit();
	displayNotes();

	displayMode();
	displayStart();
	displayCopies();
}

void EventsScreen::function(int i)
{
	init();
	
	auto fromSequence = sequencer->getActiveSequence();
	auto toSequence = sequencer->getSequence(toSq).lock();
	
	switch (i)
	{
	// Intentional fall-through
	case 1:
	case 2:
	case 3:
		tab = i;
		openScreen(tabNames[tab]);
		break;
	case 5:
	{
		auto sourceStart = time0;
		auto sourceEnd = time1;
		auto segLength = sourceEnd - sourceStart;
		auto sourceTrack = sequencer->getActiveTrack();

		if (editFunctionNumber == 0)
		{
			auto destStart = start;
			auto destOffset = destStart - sourceStart;
			
			if (!toSequence->isUsed())
				toSequence->init(fromSequence->getLastBarIndex());

			auto destTrack = toSequence->getTrack(toTr);

			if (!modeMerge)
			{
				for (auto& e : destTrack->getEvents())
				{
					auto tick = e.lock()->getTick();

					if (tick >= destOffset && tick < destOffset + (segLength * copies))
						destTrack->removeEvent(e);
				}
			}

			for (auto& e : sourceTrack->getEvents())
			{
				auto event = e.lock();
				auto ne = dynamic_pointer_cast<NoteEvent>(event);
			
				if (ne)
				{
					if (sourceTrack->getBus() == 0)
					{
						if (ne->getNote() < note0 || ne->getNote() > note1)
							continue;
					}
					else
					{
						if (note0 != 34 && note0 != ne->getNote())
							continue;
					}
				}

				if (event->getTick() >= sourceEnd)
					break;

				if (event->getTick() >= sourceStart)
				{
					for (int copy = 0; copy < copies; copy++)
					{
						int tickCandidate = event->getTick() + destOffset + (copy * segLength);
						
						if (tickCandidate < toSequence->getLastTick())
						{
							auto temp = destTrack->cloneEventIntoTrack(event).lock();
							temp->setTick(tickCandidate);
						}
					}
				}
			}

			destTrack->sortEvents();
		}
		else if (editFunctionNumber == 1)
		{
			for (auto& e : sourceTrack->getEvents())
			{
				auto event = e.lock();
			
				auto noteEvent = dynamic_pointer_cast<NoteEvent>(event);
				
				if (noteEvent)
				{
					if (durationMode == 0)
					{
						noteEvent->setDuration(noteEvent->getDuration() + durationValue);
					}
					else if (durationMode == 1)
					{
						noteEvent->setDuration(noteEvent->getDuration() - durationValue);
					}
					else if (durationMode == 2)
					{
						noteEvent->setDuration(noteEvent->getDuration() * durationValue * 0.01);
					}
					else if (durationMode == 3)
					{
						noteEvent->setDuration(durationValue);
					}
				}
			}
		}
		else if (editFunctionNumber == 2)
 {
			for (auto& e : sourceTrack->getEvents())
			{
				auto event = e.lock();
			
				auto n = dynamic_pointer_cast<NoteEvent>(event);
				
				if (n)
				{
					if (velocityMode == 0)
					{
						n->setVelocity(n->getVelocity() + velocityValue);
					}
					else if (velocityMode == 1)
					{
						n->setVelocity(n->getVelocity() - velocityValue);
					}
					else if (velocityMode == 2)
					{
						n->setVelocity((n->getVelocity() * velocityValue * 0.01));
					}
					else if (velocityMode == 3)
					{
						n->setVelocity(velocityValue);
					}
				}
			}
		}
		else if (editFunctionNumber == 3)
        {
            // The original does not process DRUM tracks.
            // We do, because it's nice and doesn't bother anyone,
            // so you won't see any filtering of that kind here.
			for (auto& e : sourceTrack->getEvents())
			{
				auto event = e.lock();
				auto n = dynamic_pointer_cast<NoteEvent>(event);
			
				if (n)
				{
					n->setNote(n->getNote() + transposeAmount);
				}
			}
		}

		openScreen("sequencer");
	}
	break;
	}
}

void EventsScreen::turnWheel(int i)
{
	init();
	auto toSequence = sequencer->getSequence(toSq).lock();

    if (checkAllTimesAndNotes(mpc, i, sequencer->getActiveSequence().get(), sequencer->getActiveTrack().get()))
    {
        return;
    }

    if (param == "start0")
	{
		setStart(SeqUtil::setBar(SeqUtil::getBar(toSequence.get(), start) + i, toSequence.get(), start));
	}
	else if (param == "start1")
	{
		setStart(SeqUtil::setBeat(SeqUtil::getBeat(toSequence.get(), start) + i, toSequence.get(), start));
	}
	else if (param == "start2")
	{
		setStart(SeqUtil::setClock(SeqUtil::getClock(toSequence.get(), start) + i, toSequence.get(), start));
	}
	else if (param == "edit")
	{
		setEdit(editFunctionNumber + i);
	}
	else if (param == "from-sq")
	{
		setFromSq(sequencer->getActiveSequenceIndex() + i);
		
		auto fromSeq = sequencer->getActiveSequence();
		
		if (time1 > fromSeq->getLastTick())
			setTime1(fromSeq->getLastTick());
	}
	else if (param == "from-tr")
	{
		setFromTr(sequencer->getActiveTrackIndex() +i);
	}
	else if (param == "to-sq")
	{
		setToSq(toSq + i);
		auto toSeq = sequencer->getSequence(toSq).lock();
		
		if (start > toSeq->getLastTick())
			setStart(toSeq->getLastTick());
	}
	else if (param == "to-tr")
	{
		setToTr(toTr + i);
	}
	else if (param == "mode")
	{
		if (editFunctionNumber == 0)
		{
			setModeMerge(i > 0);
		}
		else if (editFunctionNumber == 1)
		{
			setDurationMode(durationMode + i);
		}
		else if (editFunctionNumber == 2)
		{
			setVelocityMode(velocityMode + i);
		}
		else if (editFunctionNumber == 3)
		{
			setTransposeAmount(transposeAmount + i);
		}
	}
	else if (param == "copies")
	{
		if (editFunctionNumber == 0)
		{
			setCopies(copies + i);
		}
		else if (editFunctionNumber == 1)
		{
			setDuration(durationValue + i);
		}
		else if (editFunctionNumber == 2)
		{
			setVelocityValue(velocityValue + i);
		}
	}
}

void EventsScreen::displayStart()
{
	auto seq = sequencer->getSequence(toSq).lock();
	findField("start0").lock()->setTextPadded(SeqUtil::getBar(seq.get(), start) + 1, "0");
	findField("start1").lock()->setTextPadded(SeqUtil::getBeat(seq.get(), start) + 1, "0");
	findField("start2").lock()->setTextPadded(SeqUtil::getClock(seq.get(), start), "0");
}

void EventsScreen::displayTime()
{
	auto seq = sequencer->getActiveSequence();
	findField("time0").lock()->setTextPadded(SeqUtil::getBar(seq.get(), time0) + 1, "0");
	findField("time1").lock()->setTextPadded(SeqUtil::getBeat(seq.get(), time0) + 1, "0");
	findField("time2").lock()->setTextPadded(SeqUtil::getClock(seq.get(), time0), "0");
	findField("time3").lock()->setTextPadded(SeqUtil::getBar(seq.get(), time1) + 1, "0");
	findField("time4").lock()->setTextPadded(SeqUtil::getBeat(seq.get(), time1) + 1, "0");
	findField("time5").lock()->setTextPadded(SeqUtil::getClock(seq.get(), time1), "0");
}

void EventsScreen::displayCopies()
{
	if (editFunctionNumber == 0)
	{
		findField("copies").lock()->setTextPadded(copies);
	}
	else if (editFunctionNumber == 1)
	{
		findField("copies").lock()->setTextPadded(durationValue);
	}
	else if (editFunctionNumber == 2)
	{
		findField("copies").lock()->setTextPadded(velocityValue);
	}
}

void EventsScreen::displayMode()
{
	if (editFunctionNumber == 0)
	{
		findField("mode").lock()->setText(modeMerge ? "MERGE" : "REPLACE");
	}
	if (editFunctionNumber == 1)
	{
		findField("mode").lock()->setText(modeNames[durationMode]);
	}
	if (editFunctionNumber == 2)
	{
		findField("mode").lock()->setText(modeNames[velocityMode]);
	}
	if (editFunctionNumber == 3)
	{
		if (transposeAmount == 0)
		{
			findField("mode").lock()->setTextPadded(0);
		}
		else if (transposeAmount < 0)
		{
			findField("mode").lock()->setTextPadded(transposeAmount);
		}
		else
		{
			findField("mode").lock()->setTextPadded("+" + to_string(transposeAmount));
		}
	}
}

void EventsScreen::displayEdit()
{
	findField("edit").lock()->setText(functionNames[editFunctionNumber]);

	if (editFunctionNumber == 0)
	{
		findLabel("from-sq").lock()->setLocation(132, 1);
		findField("from-sq").lock()->setLocation(findField("from-sq").lock()->getX(), 1);
		findLabel("from-tr").lock()->setLocation(findLabel("from-tr").lock()->getX(), 1);
		findField("from-tr").lock()->setLocation(findField("from-tr").lock()->getX(), 1);
		findLabel("mode").lock()->setText("Mode:");
		findLabel("from-sq").lock()->setText("From sq:");
		findField("to-sq").lock()->Hide(false);
		findField("to-tr").lock()->Hide(false);
		findField("start0").lock()->Hide(false);
		findField("start1").lock()->Hide(false);
		findField("start2").lock()->Hide(false);
		findField("copies").lock()->Hide(false);
		findLabel("to-sq").lock()->Hide(false);
		findLabel("to-tr").lock()->Hide(false);
		findLabel("start0").lock()->Hide(false);
		findLabel("start1").lock()->Hide(false);
		findLabel("start2").lock()->Hide(false);
		findLabel("copies").lock()->setText("Copies:");
		findLabel("copies").lock()->setSize(7 * 6 + 1, 7);
		findLabel("copies").lock()->setLocation(138, 39);
		findField("copies").lock()->setLocation(findField("copies").lock()->getX(), 38);
		findLabel("mode").lock()->setLocation(150, 20);
		findField("mode").lock()->setSize(7 * 6 + 1, 9);
		findField("copies").lock()->setSize(3 * 6 + 1, 9);
	}
	else if (editFunctionNumber == 1 || editFunctionNumber == 2)
	{
		findLabel("from-sq").lock()->setLocation(132, 3);
		findField("from-sq").lock()->setLocation(findField("from-sq").lock()->getX(), 3);
		findLabel("from-tr").lock()->setLocation(findLabel("from-tr").lock()->getX(), 3);
		findField("from-tr").lock()->setLocation(findField("from-tr").lock()->getX(), 3);
		findLabel("mode").lock()->setText("Mode:");
		findLabel("from-sq").lock()->setText("Edit sq:");
		findField("to-sq").lock()->Hide(true);
		findField("to-tr").lock()->Hide(true);
		findField("start0").lock()->Hide(true);
		findField("start1").lock()->Hide(true);
		findField("start2").lock()->Hide(true);
		findField("copies").lock()->Hide(false);
		findLabel("to-sq").lock()->Hide(true);
		findLabel("to-tr").lock()->Hide(true);
		findLabel("start0").lock()->Hide(true);
		findLabel("start1").lock()->Hide(true);
		findLabel("start2").lock()->Hide(true);
		findLabel("copies").lock()->setText("Value:");
		findLabel("copies").lock()->setSize(6 * 6 + 1, 7);
		findLabel("copies").lock()->setLocation(144, 35);
		findField("copies").lock()->setLocation(findField("copies").lock()->getX(), 34);
		
		if (editFunctionNumber == 2)
		{
			findField("copies").lock()->setSize(3 * 6 + 1, 9);
		}
		else
		{
			findField("copies").lock()->setSize(4 * 6 + 1, 9);
		}
		findLabel("mode").lock()->setLocation(150, 20);
		findField("mode").lock()->setSize(10 * 6 + 1, 9);
	}
	else if (editFunctionNumber == 3)
	{
		findLabel("from-sq").lock()->setLocation(132, 3);
		findField("from-sq").lock()->setLocation(findField("from-sq").lock()->getX(), 3);
		findLabel("from-tr").lock()->setLocation(findLabel("from-tr").lock()->getX(), 3);
		findField("from-tr").lock()->setLocation(findField("from-tr").lock()->getX(), 3);
		findLabel("mode").lock()->setText("Amount:");
		findLabel("from-sq").lock()->setText("Edit sq:");
		findField("to-sq").lock()->Hide(true);
		findField("to-tr").lock()->Hide(true);
		findField("start0").lock()->Hide(true);
		findField("start1").lock()->Hide(true);
		findField("start2").lock()->Hide(true);
		findField("copies").lock()->Hide(true);
		findLabel("to-sq").lock()->Hide(true);
		findLabel("to-tr").lock()->Hide(true);
		findLabel("start0").lock()->Hide(true);
		findLabel("start1").lock()->Hide(true);
		findLabel("start2").lock()->Hide(true);
		findLabel("copies").lock()->setText("(Except drum track)");
		findLabel("copies").lock()->setLocation(132, 38);
		findLabel("copies").lock()->setSize(113, 7);
		findLabel("mode").lock()->setLocation(138, 20);
		findField("mode").lock()->setSize(3 * 6 + 1, 9);
	}
	
	displayCopies();
	displayFromSq();
	displayFromTr();
	displayMode();
	displayStart();
	displayToSq();
	displayToTr();
}

void EventsScreen::displayNotes()
{
	init();
	
	if (sequencer->getActiveTrack()->getBus() == 0)
	{
		findField("note0").lock()->setSize(47, 9);
		findField("note1").lock()->Hide(false);
		findLabel("note1").lock()->Hide(false);
		findField("note0").lock()->setAlignment(Alignment::Centered, 18);
		displayMidiNotes();
	}
	else
	{
		findField("note0").lock()->setSize(37, 9);
		findField("note1").lock()->Hide(true);
		findLabel("note1").lock()->Hide(true);
		findField("note0").lock()->setAlignment(Alignment::None);
		displayDrumNotes();
	}
}

void EventsScreen::displayMidiNotes()
{
	findField("note0").lock()->setText(StrUtil::padLeft(to_string(note0), " ", 3) + "(" + mpc::Util::noteNames()[note0] + u8"\u00D4");
	findField("note1").lock()->setText(StrUtil::padLeft(to_string(note1), " ", 3) + "(" + mpc::Util::noteNames()[note1] + u8"\u00D4");
}

void EventsScreen::displayDrumNotes()
{
	if (note0 == 34)
	{
		findField("note0").lock()->setText("ALL");
	}
	else
	{
		auto track = sequencer->getActiveTrack();
		auto program = sampler->getProgram(sampler->getDrum(track->getBus() - 1)->getProgram()).lock();
		
		auto noteText = StrUtil::padLeft(to_string(note0), " ", 2);
		auto padName = sampler->getPadName(program->getPadIndexFromNote(note0));
		findField("note0").lock()->setText(noteText + "/" + padName);
	}
}

void EventsScreen::setEdit(int i)
{
	if (i < 0 || i > 3)
		return;

	editFunctionNumber = i;
	displayEdit();
}

void EventsScreen::setFromSq(int i)
{
	if (i < 0 || i > 98)
		return;

	sequencer->setActiveSequenceIndex(i);

	displayFromSq();
}

void EventsScreen::setFromTr(int i)
{
	if (i < 0 || i > 63)
		return;
	
	sequencer->setActiveTrackIndex(i);

	displayFromTr();
}

void EventsScreen::setToSq(int i)
{
	if (i < 0 || i > 98)
		return;

	toSq = i;
	displayToSq();
}

void EventsScreen::setToTr(int i)
{
	if (i < 0 || i > 63)
		return;

	toTr = i;
	displayToTr();
}

void EventsScreen::setModeMerge(bool b)
{
	modeMerge = b;
	displayMode();
}

void EventsScreen::setCopies(int i)
{
	if (i < 1 || i > 999)
		return;

	copies = i;
	displayCopies();
}

void EventsScreen::setDurationMode(int i)
{
	if (i < 0 || i > 3)
		return;

	durationMode = i;

	if (durationMode == 2 && durationValue > 200)
		setDuration(200);
	
	displayMode();
}

void EventsScreen::setVelocityMode(int i)
{
	if (i < 0 || i > 3)
		return;

	velocityMode = i;

	if (velocityMode != 2 && velocityValue > 127)
		setVelocityValue(127);
	
	displayMode();
}

void EventsScreen::setTransposeAmount(int i)
{
	if (i < -12 || i > 12)
		return;

	transposeAmount = i;
	// Field otherwise used for displaying mode is
	// replaced by an "Amount:" field.
	displayMode();
}

void EventsScreen::setDuration(int i)
{
	if (i < 1 || i > 9999)
		return;
	
	if (durationMode == 2 && i > 200)
		return;

	durationValue = i;
	displayCopies();
}

void EventsScreen::setVelocityValue(int i)
{
	if (i < 1 || i > 200)
		return;
	
	if (velocityMode != 2 && i > 127)
		return;

	velocityValue = i;

	// Field otherwise used for displaying "Copies:" is
	// replaced by a "Value:" field.
	displayCopies();
}

void EventsScreen::setStart(int i)
{
	start = i;
	displayStart();
}

void EventsScreen::displayFromSq()
{
	findField("from-sq").lock()->setTextPadded(sequencer->getActiveSequenceIndex() + 1);
}

void EventsScreen::displayFromTr()
{
	findField("from-tr").lock()->setTextPadded(sequencer->getActiveTrackIndex() + 1);
}

void EventsScreen::displayToSq()
{
	findField("to-sq").lock()->setTextPadded(toSq + 1);
}

void EventsScreen::displayToTr()
{
	findField("to-tr").lock()->setTextPadded(toTr + 1);
}
