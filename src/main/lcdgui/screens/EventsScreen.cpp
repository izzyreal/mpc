#include "EventsScreen.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/TimeSignature.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lcdgui/screens/UserScreen.hpp>

#include <Util.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

EventsScreen::EventsScreen(const int layerIndex)
	: ScreenComponent("events", layerIndex)
{
}

void EventsScreen::open()
{
	if (tab != 0)
	{
		ls.lock()->openScreen(tabNames[tab]);
		return;
	}

	setFromSq(sequencer.lock()->getActiveSequenceIndex());
	setToSq(sequencer.lock()->getActiveSequenceIndex());
	setFromTr(sequencer.lock()->getActiveTrackIndex());
	setToTr(sequencer.lock()->getActiveTrackIndex());

	auto seq = sequencer.lock()->getActiveSequence().lock();

	if (!seq->isUsed())
	{
		auto userScreen = dynamic_pointer_cast<UserScreen>(Screens::getScreenComponent("user"));
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
	
	auto fromSequence = sequencer.lock()->getActiveSequence().lock();
	auto toSequence = sequencer.lock()->getSequence(toSq).lock();
	
	switch (i)
	{
	// Intentional fall-through
	case 1:
	case 2:
	case 3:
		tab = i;
		ls.lock()->openScreen(tabNames[tab]);
		break;
	case 5:
	{
		auto sourceStart = time0;
		auto sourceEnd = time1;
		auto segLength = sourceEnd - sourceStart;
		auto sourceTrack = fromSequence->getTrack(fromTr).lock();

		if (editFunctionNumber == 0)
		{
			auto destStart = start;
			auto destOffset = destStart - sourceStart;
			
			if (!toSequence->isUsed())
			{
				toSequence->init(fromSequence->getLastBar());
			}

			auto destTrack = toSequence->getTrack(toTr).lock();

			if (!modeMerge)
			{
				for (auto& e : destTrack->getEvents())
				{
					auto tick = e.lock()->getTick();
					
					if (tick >= destOffset && tick < destOffset + (segLength * copies))
					{
						destTrack->removeEvent(e);
					}
				}
			}

			for (auto& e : sourceTrack->getEvents())
			{
				auto event = e.lock();
				auto ne = dynamic_pointer_cast<NoteEvent>(event);
			
				if (ne)
				{
					if (sourceTrack->getBusNumber() == 0)
					{
						if (ne->getNote() < note0 || ne->getNote() > note1)
						{
							continue;
						}
					}
					else
					{
						if (mpc.getNote() != 34 && mpc.getNote() != ne->getNote())
						{
							continue;
						}
					}
				}

				if (event->getTick() >= sourceEnd)
				{
					break;
				}

				if (event->getTick() >= sourceStart)
				{
					for (int copy = 0; copy < copies; copy++)
					{
						int tickCandidate = event->getTick() + destOffset + (copy * segLength);
						
						if (tickCandidate < toSequence->getLastTick())
						{
							auto temp = destTrack->cloneEvent(event).lock();
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
			for (auto& e : sourceTrack->getEvents())
			{
				auto event = e.lock();
				auto n = dynamic_pointer_cast<NoteEvent>(event);
			
				if (n)
				{
					n->setNote(n->getNote() + velocityValue);
					break;
				}
			}
		}
		ls.lock()->openScreen("sequencer");
	}
	break;
	}
}

void EventsScreen::turnWheel(int i)
{
	init();
	auto fromSequence = sequencer.lock()->getActiveSequence().lock();
	auto toSequence = sequencer.lock()->getSequence(toSq).lock();
	auto track = fromSequence->getTrack(fromTr).lock().get();

	if (checkAllTimesAndNotes(i, fromSequence.get(), track))
	{
		return;
	}

	if (param.compare("start0") == 0)
	{
		setStart(SeqUtil::setBar(SeqUtil::getBar(toSequence.get(), start) + i, toSequence.get(), start));
	}
	else if (param.compare("start1") == 0)
	{
		setStart(SeqUtil::setBeat(SeqUtil::getBeat(toSequence.get(), start) + i, toSequence.get(), start));
	}
	else if (param.compare("start2") == 0)
	{
		setStart(SeqUtil::setClock(SeqUtil::getClock(toSequence.get(), start) + i, toSequence.get(), start));
	}
	else if (param.compare("edit") == 0)
	{
		setEdit(editFunctionNumber + i);
	}
	else if (param.compare("from-sq") == 0)
	{
		setFromSq(sequencer.lock()->getActiveSequenceIndex() + i);
		
		auto fromSeq = sequencer.lock()->getActiveSequence().lock();
		
		if (time1 > fromSeq->getLastTick())
		{
			setTime1(fromSeq->getLastTick());
		}
	}
	else if (param.compare("from-tr") == 0)
	{
		setFromTr(fromTr + i);
	}
	else if (param.compare("to-sq") == 0)
	{
		setToSq(toSq + i);
		auto toSeq = sequencer.lock()->getSequence(toSq).lock();
		
		if (start > toSeq->getLastTick())
		{
			setStart(toSeq->getLastTick());
		}
	}
	else if (param.compare("to-tr") == 0)
	{
		setToTr(toTr + i);
	}
	else if (param.compare("mode") == 0)
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
	else if (param.compare("copies") == 0)
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
	auto seq = sequencer.lock()->getSequence(toSq).lock();
	findField("start0").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), start) + 1), "0", 3));
	findField("start1").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), start) + 1), "0", 2));
	findField("start2").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), start)), "0", 2));
}

void EventsScreen::displayTime()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findField("time0").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), time0) + 1), "0", 3));
	findField("time1").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), time0) + 1), "0", 2));
	findField("time2").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), time0)), "0", 2));
	findField("time3").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), time1) + 1), "0", 3));
	findField("time4").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), time1) + 1), "0", 2));
	findField("time5").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), time1)), "0", 2));
}

void EventsScreen::displayCopies()
{
	if (editFunctionNumber == 0)
	{
		findField("copies").lock()->setText(StrUtil::padLeft(to_string(copies), " ", 3));
	}
	else if (editFunctionNumber == 1)
	{
		findField("copies").lock()->setText(StrUtil::padLeft(to_string(durationValue), " ", 4));
	}
	else if (editFunctionNumber == 2)
	{
		findField("copies").lock()->setText(StrUtil::padLeft(to_string(velocityValue), " ", 3));
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
			findField("mode").lock()->setText("  0");
		}
		if (transposeAmount < 0)
		{
			findField("mode").lock()->setText(StrUtil::padLeft(to_string(transposeAmount), " ", 3));
		}
		if (transposeAmount > 0)
		{
			findField("mode").lock()->setText(StrUtil::padLeft("+" + to_string(transposeAmount), " ", 3));
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
		findLabel("copies").lock()->setLocation(138, 35);
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
		findLabel("copies").lock()->setSize(112, 7);
		findLabel("mode").lock()->setLocation(138, 20);
		findField("mode").lock()->setSize(3 * 6 + 1, 9);
	}
}

void EventsScreen::displayNotes()
{
	init();

	auto sequence = sequencer.lock()->getActiveSequence().lock();
	auto track = sequence->getTrack(fromTr).lock();

	if (track->getBusNumber() == 0)
	{
		findField("note0").lock()->setSize(48, 9);
		findField("note1").lock()->Hide(false);
		findLabel("note1").lock()->Hide(false);
		displayMidiNotes();
	}
	else
	{
		findField("note0").lock()->setSize(37, 9);
		findField("note1").lock()->Hide(true);
		findLabel("note1").lock()->Hide(true);
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
	auto drumNote = mpc.getNote();

	if (drumNote == 34)
	{
		findField("note0").lock()->setText("ALL");
	}
	else
	{
		auto sequence = sequencer.lock()->getActiveSequence().lock();
		auto track = sequence->getTrack(fromTr).lock();
		auto program = sampler.lock()->getProgram(sampler.lock()->getDrum(track->getBusNumber() - 1)->getProgram()).lock();
		
		auto noteText = StrUtil::padLeft(to_string(drumNote), " ", 2);
		auto padText = sampler.lock()->getPadName(program->getPadIndexFromNote(drumNote));
		
		findField("note0").lock()->setText(noteText + "/" + padText);
	}
}

void EventsScreen::setEdit(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}

	editFunctionNumber = i;
	displayEdit();
}

void EventsScreen::setFromSq(int i)
{
	if (i < 0 || i > 98)
	{
		return;
	}

	sequencer.lock()->setActiveSequenceIndex(i);

	// The below check can be removed if we're only supposed to jump to used sequences.
	// For now we initialize any sequence that we come across.
	auto seq = sequencer.lock()->getActiveSequence().lock();
	if (!seq->isUsed())
	{
		auto userScreen = dynamic_pointer_cast<UserScreen>(Screens::getScreenComponent("user"));
		seq->init(userScreen->lastBar);
	}

	displayFromSq();
}

void EventsScreen::setFromTr(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	fromTr = i;
	displayFromTr();
}

void EventsScreen::setToSq(int i)
{
	if (i < 0 || i > 98)
	{
		return;
	}

	toSq = i;
	displayToSq();
}

void EventsScreen::setToTr(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
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
	{
		return;
	}
	copies = i;
	displayCopies();
}

void EventsScreen::setDurationMode(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}

	durationMode = i;
	if (durationMode == 2 && durationValue > 200)
	{
		setDuration(200);
	}
	displayDurationMode();
}

void EventsScreen::setVelocityMode(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}
	velocityMode = i;

	if (velocityMode != 2 && velocityValue > 127)
	{
		setVelocityValue(127);
	}
	
	displayVelocityMode();
}

void EventsScreen::setTransposeAmount(int i)
{
	if (i < -12 || i > 12)
	{
		return;
	}
	transposeAmount = i;
	displayTransposeAmount();
}

void EventsScreen::setDuration(int i)
{
	if (i < 1 || i > 9999)
	{
		return;
	}
	
	if (durationMode == 2 && i > 200)
	{
		return;
	}
	durationValue = i;
	displayCopies();
}

void EventsScreen::setVelocityValue(int i)
{
	if (i < 1 || i > 200)
	{
		return;
	}
	
	if (velocityMode != 2 && i > 127)
	{
		return;
	}
	velocityValue = i;
	displayVelocityValue();
}

void EventsScreen::setStart(int i)
{
	start = i;
	displayStart();
}

void EventsScreen::displayFromSq()
{
	findField("from-sq").lock()->setText(to_string(sequencer.lock()->getActiveSequenceIndex() + 1));
}

void EventsScreen::displayFromTr()
{
	findField("from-tr").lock()->setText(to_string(fromTr + 1));
}

void EventsScreen::displayToSq()
{
	findField("to-sq").lock()->setText(to_string(toSq + 1));
}

void EventsScreen::displayToTr()
{
	findField("to-tr").lock()->setText(to_string(toTr + 1));
}

void EventsScreen::displayDurationMode()
{

}

void EventsScreen::displayVelocityMode()
{

}

void EventsScreen::displayTransposeAmount()
{

}

void EventsScreen::displayVelocityValue()
{

}
