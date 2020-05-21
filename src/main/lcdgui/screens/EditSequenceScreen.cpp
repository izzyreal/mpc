#include "EditSequenceScreen.hpp"

#include <ui/Uis.hpp>

#include <lcdgui/Label.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/TrMoveScreen.hpp>
#include <lcdgui/screens/BarCopyScreen.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TimeSignature.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lang/StrUtil.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

using namespace moduru::lang;

using namespace std;

EditSequenceScreen::EditSequenceScreen(const int& layer)
	: ScreenComponent("editsequence", layer)
{
}

void EditSequenceScreen::open()
{
	displayEdit();
	displayTime();
	displayNotes();
	findField("fromsq").lock()->setText(to_string(fromSq + 1));
	findField("tr0").lock()->setText(to_string(tr0 + 1));
	findField("tosq").lock()->setText(to_string(toSq + 1));
	findField("tr1").lock()->setText(to_string(tr1 + 1));
	displayMode();
	displayStart();
	displayCopies();
}

void EditSequenceScreen::function(int i)
{
	init();
	
	auto fromSequence = sequencer.lock()->getSequence(fromSq).lock();
	auto toSequence = sequencer.lock()->getSequence(toSq).lock();

	auto uis = Mpc::instance().getUis().lock();
	
	switch (i)
	{
	case 1:
	{
		auto barCopyScreen = dynamic_pointer_cast<BarCopyScreen>(Screens::getScreenComponent("barcopy"));
		barCopyScreen->setFromSq(fromSq);
		ls.lock()->openScreen("barcopy");
		break;
	}
	case 2:
	{
		auto trMoveScreen = dynamic_pointer_cast<TrMoveScreen>(Screens::getScreenComponent("trmove"));
		trMoveScreen->setSq(fromSq);
		ls.lock()->openScreen("trmove");
		break;
	}
	case 3:
		ls.lock()->openScreen("user");
		break;
	case 5:
	{
		auto sourceStart = time0;
		auto sourceEnd = time1;
		auto segLength = sourceEnd - sourceStart;
		auto sourceTrack = fromSequence->getTrack(tr0).lock();

		if (editFunctionNumber == 0)
		{
			auto destStart = start;
			auto destOffset = destStart - sourceStart;
			
			if (!toSequence->isUsed())
			{
				toSequence->init(fromSequence->getLastBar());
			}

			auto destTrack = toSequence->getTrack(tr1).lock();

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
						auto mn0 = getMidiNote0();
						auto mn1 = getMidiNote1();
						if (ne->getNote() < mn0 || ne->getNote() > mn1)
						{
							continue;
						}
					}
					else
					{
						if (drumNote != 34 && drumNote != ne->getNote())
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

void EditSequenceScreen::turnWheel(int i)
{
	init();
	auto fromSequence = sequencer.lock()->getSequence(fromSq).lock();
	auto toSequence = sequencer.lock()->getSequence(toSq).lock();

	checkAllTimesAndNotes(i, fromSequence.get());

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
	else if (param.compare("editfunction") == 0)
	{
		setEdit(editFunctionNumber + i);
	}
	else if (param.compare("drumnote") == 0)
	{
		setDrumNote(drumNote + i);
	}
	else if (param.compare("midinote0") == 0)
	{
		setMidiNote0(getMidiNote0() + i);
	}
	else if (param.compare("midinote1") == 0
		) {
		setMidiNote1(getMidiNote1() + i);
	}
	else if (param.compare("fromsq") == 0)
	{
		setFromSq(fromSq + i);
		
		auto fromSeq = sequencer.lock()->getSequence(fromSq).lock();
		
		if (time1 > fromSeq->getLastTick())
		{
			setTime1(fromSeq->getLastTick());
		}
	}
	else if (param.compare("tr0") == 0)
	{
		setTr0(tr0 + i);
	}
	else if (param.compare("tosq") == 0)
	{
		setToSq(toSq + i);
		auto toSeq = sequencer.lock()->getSequence(toSq).lock();
		
		if (start > toSeq->getLastTick())
		{
			setStart(toSeq->getLastTick());
		}
	}
	else if (param.compare("tr1") == 0)
	{
		setTr1(tr1 + i);
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
	else if (param.compare("copies") == 0) {
		if (editFunctionNumber == 0)
		{
			setCopies(copies + i);
		}
		else if (editFunctionNumber == 1) {
			setDuration(durationValue + i);
		}
		else if (editFunctionNumber == 2) {
			setVelocityValue(velocityValue + i);
		}
	}
}

void EditSequenceScreen::displayStart()
{
	auto seq = sequencer.lock()->getSequence(toSq).lock();
	findField("start0").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), start) + 1), "0", 3));
	findField("start1").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), start) + 1), "0", 2));
	findField("start2").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), start)), "0", 2));
}

void EditSequenceScreen::displayTime()
{
	auto seq = sequencer.lock()->getSequence(fromSq).lock();
	findField("time0").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), time0) + 1), "0", 3));
	findField("time1").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), time0) + 1), "0", 2));
	findField("time2").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), time0)), "0", 2));
	findField("time3").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), time1) + 1), "0", 3));
	findField("time4").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), time1) + 1), "0", 2));
	findField("time5").lock()->setText(StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), time1)), "0", 2));
}

void EditSequenceScreen::displayCopies()
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

void EditSequenceScreen::displayMode()
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

void EditSequenceScreen::displayEdit()
{
	findField("editfunction").lock()->setText(functionNames[editFunctionNumber]);

	if (editFunctionNumber == 0)
	{
		findLabel("fromsq").lock()->setLocation(132, 1);
		findField("fromsq").lock()->setLocation(findField("fromsq").lock()->getX(), 1);
		findLabel("tr0").lock()->setLocation(findLabel("tr0").lock()->getX(), 1);
		findField("tr0").lock()->setLocation(findField("tr0").lock()->getX(), 1);
		findLabel("mode").lock()->setText("Mode:");
		findLabel("fromsq").lock()->setText("From sq:");
		findField("tosq").lock()->Hide(false);
		findField("tr1").lock()->Hide(false);
		findField("start0").lock()->Hide(false);
		findField("start1").lock()->Hide(false);
		findField("start2").lock()->Hide(false);
		findField("copies").lock()->Hide(false);
		findLabel("tosq").lock()->Hide(false);
		findLabel("tr1").lock()->Hide(false);
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
		findLabel("fromsq").lock()->setLocation(132, 3);
		findField("fromsq").lock()->setLocation(findField("fromsq").lock()->getX(), 3);
		findLabel("tr0").lock()->setLocation(findLabel("tr0").lock()->getX(), 3);
		findField("tr0").lock()->setLocation(findField("tr0").lock()->getX(), 3);
		findLabel("mode").lock()->setText("Mode:");
		findLabel("fromsq").lock()->setText("Edit sq:");
		findField("tosq").lock()->Hide(true);
		findField("tr1").lock()->Hide(true);
		findField("start0").lock()->Hide(true);
		findField("start1").lock()->Hide(true);
		findField("start2").lock()->Hide(true);
		findField("copies").lock()->Hide(false);
		findLabel("tosq").lock()->Hide(true);
		findLabel("tr1").lock()->Hide(true);
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
		findLabel("fromsq").lock()->setLocation(132, 3);
		findField("fromsq").lock()->setLocation(findField("fromsq").lock()->getX(), 3);
		findLabel("tr0").lock()->setLocation(findLabel("tr0").lock()->getX(), 3);
		findField("tr0").lock()->setLocation(findField("tr0").lock()->getX(), 3);
		findLabel("mode").lock()->setText("Amount:");
		findLabel("fromsq").lock()->setText("Edit sq:");
		findField("tosq").lock()->Hide(true);
		findField("tr1").lock()->Hide(true);
		findField("start0").lock()->Hide(true);
		findField("start1").lock()->Hide(true);
		findField("start2").lock()->Hide(true);
		findField("copies").lock()->Hide(true);
		findLabel("tosq").lock()->Hide(true);
		findLabel("tr1").lock()->Hide(true);
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

void EditSequenceScreen::displayNotes()
{
	init();

	findField("midinote0").lock()->setSize(8 * 6, 9);
	findField("midinote1").lock()->setSize(8 * 6, 9);
	findField("midinote1").lock()->setLocation(62, 39);

	if (track.lock()->getBusNumber() == 0)
	{
		findField("drumnote").lock()->Hide(true);
		findField("midinote0").lock()->Hide(false);
		findField("midinote1").lock()->Hide(false);
		findLabel("midinote1").lock()->Hide(false);
		displayMidiNotes();
	}
	else
	{
		findField("drumnote").lock()->Hide(false);
		findField("midinote0").lock()->Hide(true);
		findField("midinote1").lock()->Hide(true);
		findLabel("midinote1").lock()->Hide(true);
		displayDrumNotes();
	}
}

void EditSequenceScreen::displayMidiNotes()
{
	//findField("midinote0").lock()->setText(StrUtil::padLeft(to_string(getMidiNote0()), " ", 3) + "(" + mpc::ui::Uis::noteNames[getMidiNote0()] + u8"\u00D4");
	//findField("midinote1").lock()->setText(StrUtil::padLeft(to_string(getMidiNote1()), " ", 3) + "(" + mpc::ui::Uis::noteNames[getMidiNote1()] + u8"\u00D4");
}

void EditSequenceScreen::displayDrumNotes()
{
	auto sequence = sequencer.lock()->getSequence(fromSq).lock();
	auto track = sequence->getTrack(tr0).lock();
	auto program = sampler.lock()->getProgram(sampler.lock()->getDrum(track->getBusNumber() - 1)->getProgram()).lock();
	
	if (drumNote == 34)
	{
		findField("drumnote").lock()->setText("ALL");
	}
	else
	{
		auto padNumber = StrUtil::padLeft(to_string(drumNote), " ", 2);
		auto padName = sampler.lock()->getPadName(program->getPadNumberFromNote(drumNote));
		findField("drumnote").lock()->setText(padNumber + "/" + padName);
	}
}

void EditSequenceScreen::setEdit(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}
	editFunctionNumber = i;
	displayEdit();
}

void EditSequenceScreen::setDrumNote(int i)
{
	if (i < 34 || i > 98)
	{
		return;
	}
	drumNote = i;
	displayDrumNotes();
}

void EditSequenceScreen::setFromSq(int i)
{
	if (i < 0 || i > 98)
	{
		return;
	}
	fromSq = i;
	displayFromSq();
}

void EditSequenceScreen::setTr0(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	tr0 = i;
	displayTr0();
}

void EditSequenceScreen::setToSq(int i)
{
	if (i < 0 || i > 98)
	{
		return;
	}

	toSq = i;
	displayToSq();
}

void EditSequenceScreen::setTr1(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	tr1 = i;
	displayTr1();
}

void EditSequenceScreen::setModeMerge(bool b)
{
	modeMerge = b;
	displayMode();
}

void EditSequenceScreen::setCopies(int i)
{
	if (i < 1 || i > 999)
	{
		return;
	}
	copies = i;
	displayCopies();
}

void EditSequenceScreen::setDurationMode(int i)
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

void EditSequenceScreen::setVelocityMode(int i)
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

void EditSequenceScreen::setTransposeAmount(int i)
{
	if (i < -12 || i > 12)
	{
		return;
	}
	transposeAmount = i;
	displayTransposeAmount();
}

void EditSequenceScreen::setDuration(int i)
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

void EditSequenceScreen::setVelocityValue(int i)
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

void EditSequenceScreen::setStart(int i)
{
	start = i;
	displayStart();
}

void EditSequenceScreen::displayTr1()
{
	findField("tr1").lock()->setText(to_string(tr1 + 1));
}

void EditSequenceScreen::displayToSq()
{
	findField("tosq").lock()->setText(to_string(toSq + 1));
}

void EditSequenceScreen::displayTr0()
{
	findField("tr0").lock()->setText(to_string(tr0 + 1));
}

void EditSequenceScreen::displayFromSq()
{
	findField("fromsq").lock()->setText(to_string(fromSq + 1));
}

void EditSequenceScreen::displayDurationMode()
{

}

void EditSequenceScreen::displayVelocityMode()
{

}

void EditSequenceScreen::displayTransposeAmount()
{

}

void EditSequenceScreen::displayVelocityValue()
{

}
