#include "StepEditorScreen.hpp"

#include <audiomidi/EventHandler.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

const int EVENT_ROW_COUNT = 4;

StepEditorScreen::StepEditorScreen(const int layerIndex)
	: ScreenComponent("step-editor", layerIndex)
{
	for (int i = 0; i < EVENT_ROW_COUNT; i++)
	{
		addChild(make_shared<EventRow>(i)).lock();
	}
}

void StepEditorScreen::open()
{
	findField("controlnumber").lock()->Hide(true);
	findField("fromnote").lock()->Hide(true);
	findField("tonote").lock()->Hide(true);
	findLabel("controlnumber").lock()->Hide(true);

	init();

	if (track.lock()->getBus() != 0)
	{
		int pgm = sampler.lock()->getDrumBusProgramNumber(track.lock()->getBus());
		program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pgm).lock());
	}

	refreshViewNotes();
	setViewNotesText();
	displayView();
	sequencer.lock()->addObserver(this);
	track.lock()->addObserver(this);
	
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");

	initVisibleEvents();

	refreshEventRows();
	refreshSelection();

	ls.lock()->setFocus("a0");
}

void StepEditorScreen::close()
{
	init();
	sequencer.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);

	track.lock()->removeDoubles();

	for (auto& e : visibleEvents)
	{
		if (e.lock())
		{
			e.lock()->deleteObserver(this);
		}
	}
}

void StepEditorScreen::function(int i)
{
	init();

	switch (i)
	{
	case 0:
		ls.lock()->openScreen("step-timing-correct");
		break;
	case 1:
		if (selectionStartIndex != -1)
		{
			// CopySelectedNotes
			setSelectedEvents();
			placeHolder = selectedEvents;
			clearSelection();
		}
		else if (selectionStartIndex == -1 && param.length() == 2)
		{
			// CopySelectedNote
			auto eventNumber = stoi(param.substr(1, 1));
			placeHolder = { visibleEvents[eventNumber] };
		}
		break;
	case 2:
		if (selectionStartIndex != -1)
		{
			removeEvents();

			ls.lock()->setFocus("a0");
		}
		else if (param.length() == 2)
		{
			auto rowIndex = stoi(param.substr(1, 2));
		
			if (!dynamic_pointer_cast<EmptyEvent>(visibleEvents[rowIndex].lock()))
			{
				for (int i = 0; i < track.lock()->getEvents().size(); i++)
				{
					if (track.lock()->getEvents()[i].lock() == visibleEvents[rowIndex].lock())
					{
						track.lock()->removeEvent(i);
						break;
					}
				}

				// Here we check whether the removed event was the last (non-empty) visible event.
				// If yes, we make sure the empty event's field has focus.
				if (dynamic_pointer_cast<EmptyEvent>(visibleEvents[rowIndex + 1].lock()))
				{
					ls.lock()->setFocus("a" + to_string(rowIndex));
				}

				if (rowIndex == 2 && yOffset > 0)
				{
					yOffset--;
				}
			}
		}

		initVisibleEvents();
		refreshEventRows();
		refreshSelection();

		break;
	case 3:
	{
		bool posIsLastTick = sequencer.lock()->getTickPosition() == sequencer.lock()->getActiveSequence().lock()->getLastTick();
		if (selectionEndIndex == -1)
		{
			if (!posIsLastTick)
			{
				ls.lock()->openScreen("insert-event");
			}
		}
		else
		{
			auto eventNumber = stoi(param.substr(1, 2));
			auto eventLetter = param.substr(0, 1);

			auto ne = dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber].lock());
			auto pbe = dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber].lock());
			auto pce = dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber].lock());
			auto cpe = dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber].lock());
			auto ppe = dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber].lock());
			auto cce = dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber].lock());
			auto sysex = dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber].lock());
			auto me = dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber].lock());

			if (pbe || me || sysex)
			{
				return;
			}

			bool isA = eventLetter.compare("a") == 0;
			bool isB = eventLetter.compare("b") == 0;
			bool isC = eventLetter.compare("c") == 0;
			bool isD = eventLetter.compare("d") == 0;
			bool isE = eventLetter.compare("e") == 0;

			if ((ppe || cce) && isA)
			{
				return;
			}

			if (ne && track.lock()->getBus() != 0)
			{
				if (isA)
				{
					setChangeNoteToIndex(ne->getNote());
				}
				else if (isB)
				{
					setChangeVariationTypeNumber(ne->getVariationTypeNumber());
				}
				else if (isC)
				{
					setChangeVariationTypeNumber(ne->getVariationTypeNumber());
					setChangeVariationValue(ne->getVariationValue());
				}
				else if (isD)
				{
					setEditValue(ne->getDuration());
				}
				else if (isE)
				{
					setEditValue(ne->getVelocity());
				}
			}

			if (ne && track.lock()->getBus() == 0)
			{
				if (isA)
				{
					setChangeNoteToIndex(ne->getNote());
				}
				else if (isB)
				{
					setEditValue(ne->getDuration());
				}
				else if (isC)
				{
					setEditValue(ne->getVelocity());
				}
			}
			else if (pce)
			{
				setEditValue(pce->getProgram());
			}
			else if (cpe)
			{
				setEditValue(cpe->getAmount());
			}
			else if (ppe)
			{
				setEditValue(ppe->getAmount());
			}
			else if (cce)
			{
				setEditValue(cce->getAmount());
			}

			setSelectedEvent(visibleEvents[eventNumber]);
			setSelectedEvents();
			setSelectedParameterLetter(eventLetter);
			ls.lock()->openScreen("edit-multiple");
			break;
		}
	}
	case 4:
		if (placeHolder.size() != 0)
			ls.lock()->openScreen("paste-event");
		break;
	case 5:
		if (selectionStartIndex == -1)
		{
			if (param.length() == 2)
			{
				auto eventNumber = stoi(param.substr(1, 2));
				auto event = visibleEvents[eventNumber].lock();
				auto empty = dynamic_pointer_cast<EmptyEvent>(event);
			
				if (!empty)
				{
					auto tick = event->getTick();
					event->setTick(-1);
					mpc.getEventHandler().lock()->handle(event, track.lock().get());
					event->setTick(tick);
				}
			}
		}
		else
		{
			clearSelection();
		}
		break;
	}
}

void StepEditorScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("view") == 0)
	{
		setView(view + i);
	}
	else if (param.compare("now0") == 0)
	{
		sequencer.lock()->setBar(sequencer.lock()->getCurrentBarIndex() + i);
	}
	else if (param.compare("now1") == 0)
	{
		sequencer.lock()->setBeat(sequencer.lock()->getCurrentBeatIndex() + i);
	}
	else if (param.compare("now2") == 0)
	{
		sequencer.lock()->setClock(sequencer.lock()->getCurrentClockNumber() + i);
	}
	else if (param.compare("tcvalue") == 0)
	{
		auto screen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"));
		auto noteValue = screen->getNoteValue();
		screen->setNoteValue(noteValue + i);
	}
	else if (param.compare("fromnote") == 0)
	{
		if (track.lock()->getBus() != 0) setFromNotePad(fromNotePad + i);
		if (track.lock()->getBus() == 0) setNoteA(noteA + i);
	}
	else if (param.compare("tonote") == 0)
	{
		setNoteB(noteB + i);
	}
	else if (param.compare("controlnumber") == 0)
	{
		setControl(control + i);
	}
	else if (param.length() == 2)
	{

		auto eventNumber = stoi(param.substr(1, 2));

		auto sysEx = dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber].lock());
		auto channelPressure = dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber].lock());
		auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber].lock());
		auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber].lock());
		auto programChange = dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber].lock());
		auto pitchBend = dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber].lock());
		auto mixer = dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber].lock());
		auto note = dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber].lock());

		if (sysEx)
		{
			if (param.find("a") != string::npos)
			{
				sysEx->setByteA(sysEx->getByteA() + i);
			}
			else if (param.find("b") != string::npos)
			{
				sysEx->setByteB(sysEx->getByteB() + i);
			}
		}
		else if (channelPressure)
		{
			if (param.find("a") != string::npos)
			{
				channelPressure->setAmount(channelPressure->getAmount() + i);
			}
		}
		else if (polyPressure)
		{
			if (param.find("a") != string::npos)
			{
				polyPressure->setNote(polyPressure->getNote() + i);
			}
			else if (param.find("b") != string::npos)
			{
				polyPressure->setAmount(polyPressure->getAmount() + i);
			}
		}
		else if (controlChange)
		{
			if (param.find("a") != string::npos)
			{
				controlChange->setController(controlChange->getController() + i);
			}
			else if (param.find("b") != string::npos)
			{
				controlChange->setAmount(controlChange->getAmount() + i);
			}

		}
		else if (programChange)
		{
			if (param.find("a") != string::npos)
			{
				programChange->setProgram(programChange->getProgram() + i);
			}

		}
		else if (pitchBend)
		{
			if (param.find("a") != string::npos)
			{
				pitchBend->setAmount(pitchBend->getAmount() + i);
			}

		}
		else if (mixer)
		{
			if (param.find("a") != string::npos)
			{
				mixer->setParameter(mixer->getParameter() + i);
			}
			else if (param.find("b") != string::npos)
			{
				mixer->setPadNumber(mixer->getPad() + i);
			}
			else if (param.find("c") != string::npos)
			{
				mixer->setValue(mixer->getValue() + i);
			}
		}
		else if (note && track.lock()->getBus() == 0)
		{
			if (param.find("a") != string::npos)
			{
				note->setNote(note->getNote() + i);
			}
			else if (param.find("b") != string::npos)
			{
				note->setDuration(note->getDuration() + i);
			}
			else if (param.find("c") != string::npos)
			{
				note->setVelocity(note->getVelocity() + i);
			}
		}
		else if (note && track.lock()->getBus() != 0)
		{
			if (param.find("a") != string::npos)
			{
				if (note->getNote() + i > 98)
				{
					if (note->getNote() != 98) note->setNote(98);
					return;
				}
				else if (note->getNote() + i < 35)
				{
					if (note->getNote() != 35) note->setNote(35);
					return;
				}
				else if (note->getNote() < 35)
				{
					note->setNote(35);
					return;
				}
				else if (note->getNote() > 98)
				{
					note->setNote(98);
					return;
				}
				
				note->setNote(note->getNote() + i);
			}
			else if (param.find("b") != string::npos)
			{
				note->setVariationTypeNumber(note->getVariationTypeNumber() + i);
			}
			else if (param.find("c") != string::npos)
			{
				note->setVariationValue(note->getVariationValue() + i);
			}
			else if (param.find("d") != string::npos)
			{
				note->setDuration(note->getDuration() + i);
			}
			else if (param.find("e") != string::npos)
			{
				note->setVelocity(note->getVelocity() + i);
			}
		}
	}
	refreshSelection();
}

void StepEditorScreen::prevStepEvent()
{
	init();
	
	auto controls = mpc.getControls().lock();

	if (controls->isGoToPressed())
	{
		sequencer.lock()->goToPreviousEvent();
	}
	else {
		sequencer.lock()->goToPreviousStep();
	}
}

void StepEditorScreen::nextStepEvent()
{
	init();
	
	auto controls = mpc.getControls().lock();
	
	if (controls->isGoToPressed())
	{
		sequencer.lock()->goToNextEvent();
	}
	else {
		sequencer.lock()->goToNextStep();
	}
}

void StepEditorScreen::prevBarStart()
{
	init();
	auto controls = mpc.getControls().lock();
	
	if (controls->isGoToPressed())
	{
		sequencer.lock()->setBar(0);
	}
	else {
		sequencer.lock()->setBar(sequencer.lock()->getCurrentBarIndex() - 1);
	}
}

void StepEditorScreen::nextBarEnd()
{
	init();
	
	auto controls = mpc.getControls().lock();
	
	if (controls->isGoToPressed())
	{
		sequencer.lock()->setBar(sequencer.lock()->getActiveSequence().lock()->getLastBarIndex() + 1);
	}
	else {
		sequencer.lock()->setBar(sequencer.lock()->getCurrentBarIndex() + 1);
	}
}

void StepEditorScreen::left()
{
	BaseControls::left();
	checkSelection();
	refreshSelection();
}

void StepEditorScreen::right()
{
	BaseControls::right();
	checkSelection();
	refreshSelection();
}

void StepEditorScreen::up()
{
	init();
	
	if (param.length() == 2)
	{
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto controls = mpc.getControls().lock();
		
		if (!controls->isShiftPressed() && srcNumber == 0 && yOffset == 0)
		{
			clearSelection();
			lastColumn = srcLetter;
			ls.lock()->setFocus("view");
			refreshSelection();
			return;
		}
		
		if (srcNumber == 0 && yOffset != 0)
		{
			setyOffset(yOffset - 1);

			if (controls->isShiftPressed())
			{
				setSelectionEndIndex(srcNumber + yOffset);
			}
		}

		downOrUp(-1);
	}
}

void StepEditorScreen::down()
{
	init();

	if (param.compare("view") == 0 || param.find("now") != string::npos)
	{
		if (dynamic_pointer_cast<EmptyEvent>(visibleEvents[0].lock()))
		{
			lastColumn = "a";
		}

		ls.lock()->setFocus(lastColumn + "0");
		return;
	}
	
	if (param.length() == 2)
	{
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto controls = mpc.getControls().lock();
		
		if (srcNumber == 3)
		{
			if (yOffset + 4 == eventsAtCurrentTick.size())
			{
				return;
			}
			
			setyOffset(yOffset + 1);

			auto focus = ls.lock()->getFocus();
			auto comp = findField(focus).lock();

			if (comp->IsHidden())
			{
				ls.lock()->setFocus("a3");
			}

			if (controls->isShiftPressed() && dynamic_pointer_cast<EmptyEvent>(visibleEvents[3].lock()))
			{
				setSelectionEndIndex(srcNumber + yOffset);
			}
			
			refreshSelection();
			return;
		}

		downOrUp(1);
	}
}

void StepEditorScreen::shift()
{
	init();
	BaseControls::shift();
	if (param.length() == 2)
	{
		auto eventNumber = stoi(param.substr(1, 2));
		setSelectionStartIndex(eventNumber + yOffset);
	}
}

void StepEditorScreen::downOrUp(int increment)
{
	if (param.length() == 2)
	{
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto controls = mpc.getControls().lock();
		auto destination = srcLetter + to_string(srcNumber + increment);
		
		if (srcNumber + increment != -1)
		{
			if (!(controls->isShiftPressed() && dynamic_pointer_cast<EmptyEvent>(visibleEvents[(int)(srcNumber + increment)].lock())))
			{
				auto tf = findField(destination).lock();
				if (tf)
				{
					if (!tf->IsHidden())
					{
						ls.lock()->setFocus(tf->getName());
					}
					else
					{
						ls.lock()->setFocus("a" + to_string(srcNumber + increment));
					}
				}
			}
		}

		if (controls->isShiftPressed())
		{
			setSelectionEndIndex(srcNumber + increment + yOffset);
		}
		else {
			checkSelection();
			refreshSelection();
		}
	}
}

void StepEditorScreen::refreshSelection()
{
	auto firstEventIndex = selectionStartIndex;
	auto lastEventIndex = selectionEndIndex;
	bool somethingSelected = false;
	
	if (firstEventIndex != -1)
	{
		if (firstEventIndex > lastEventIndex)
		{
			firstEventIndex = selectionEndIndex;
			lastEventIndex = selectionStartIndex;
		}

		for (int i = 0; i < EVENT_ROW_COUNT; i++)
		{
			int absoluteEventNumber = i + yOffset;
			auto eventRow = findEventRows()[i].lock();

			if (absoluteEventNumber >= firstEventIndex && absoluteEventNumber < lastEventIndex + 1)
			{
				eventRow->setSelected(true);
				somethingSelected = true;
			}
			else
			{
				eventRow->setSelected(false);
			}
		}
	}
	else
	{
		for (int i = 0; i < EVENT_ROW_COUNT; i++)
		{
			auto eventRow = findEventRows()[i].lock();
			eventRow->setSelected(false);
		}
	}

	if (somethingSelected)
	{
		ls.lock()->setFunctionKeysArrangement(1);
	}
}

void StepEditorScreen::initVisibleEvents()
{
	init();

	for (auto& e : eventsAtCurrentTick)
	{
		if (e.lock()) e.lock()->deleteObserver(this);
	}
	
	eventsAtCurrentTick.clear();
	
	auto lSequencer = sequencer.lock();
	
	for (auto& event : track.lock()->getEvents())
	{
		auto lEvent = event.lock();

		if (lEvent->getTick() == sequencer.lock()->getTickPosition())
		{
			if ((view == 0
				|| view == 1)
				&& dynamic_pointer_cast<NoteEvent>(lEvent))
			{
				auto ne = dynamic_pointer_cast<NoteEvent>(lEvent);
			
				if (track.lock()->getBus() != 0)
				{
					if (fromNotePad == 34)
					{
						eventsAtCurrentTick.push_back(ne);
					}
					else if (fromNotePad != 34
						&& fromNotePad == ne->getNote())
					{
						eventsAtCurrentTick.push_back(ne);
					}
				}
				else {
					if ( (ne->getNote() >= noteA
						&& ne->getNote() <= noteB)
						|| view == 0)
					{
						eventsAtCurrentTick.push_back(ne);
					}
				}
			}

			if ((view == 0
				|| view == 2)
				&& dynamic_pointer_cast<PitchBendEvent>(lEvent))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0 || view == 3)
				&& dynamic_pointer_cast<ControlChangeEvent>(lEvent))
			{
				if (control == -1) {
					eventsAtCurrentTick.push_back(event);
				}
				if (control == dynamic_pointer_cast<ControlChangeEvent>(lEvent)->getController())
				{
					eventsAtCurrentTick.push_back(event);
				}
			}
			if ((view == 0
				|| view == 4)
				&& dynamic_pointer_cast<ProgramChangeEvent>(lEvent))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0
				|| view == 5)
				&& dynamic_pointer_cast<ChannelPressureEvent>(lEvent))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0
				|| view == 6)
				&& dynamic_pointer_cast<PolyPressureEvent>(lEvent))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0
				|| view == 7)
				&& (dynamic_pointer_cast<SystemExclusiveEvent>(lEvent)
					|| dynamic_pointer_cast<MixerEvent>(lEvent)))
			{
				eventsAtCurrentTick.push_back(event);
			}

		}
	}

	eventsAtCurrentTick.push_back(emptyEvent);
	
	for (auto& e : visibleEvents)
	{
		if (e.lock())
		{
			e.lock()->deleteObserver(this);
		}
	}
	
	visibleEvents = vector<weak_ptr<Event>>(4);
	int firstVisibleEventIndex = yOffset;
	int visibleEventCounter = 0;
	
	for (int i = 0; i < EVENT_ROW_COUNT; i++)
	{
		visibleEvents[visibleEventCounter] = eventsAtCurrentTick[i + firstVisibleEventIndex];
		visibleEventCounter++;

		if (visibleEventCounter > 3 || visibleEventCounter > eventsAtCurrentTick.size() - 1)
		{
			break;
		}
	}
}

void StepEditorScreen::refreshEventRows()
{
	for (int i = 0; i < EVENT_ROW_COUNT; i++)
	{
		auto eventRow = findEventRows()[i].lock();
		auto event = visibleEvents[i].lock();
	
		if (event)
		{
			eventRow->Hide(false);
			event->addObserver(this);
			eventRow->setBus(sequencer.lock()->getActiveTrack().lock()->getBus());
		}
		else
		{
			eventRow->Hide(true);
		}

		eventRow->setEvent(event);
		eventRow->init();
	}
}

void StepEditorScreen::refreshViewNotes()
{
	init();
	
	if (view == 1 && track.lock()->getBus() != 0)
	{
		findLabel("fromnote").lock()->Hide(false);
		findField("fromnote").lock()->Hide(false);
		findField("fromnote").lock()->setLocation(67, 0);
		findLabel("tonote").lock()->Hide(true);
		findField("tonote").lock()->Hide(true);
	}
	else if (view == 1 && track.lock()->getBus() == 0)
	{
		findLabel("fromnote").lock()->Hide(false);
		findField("fromnote").lock()->Hide(false);
		findField("fromnote").lock()->setLocation(62, 0);
		findField("fromnote").lock()->setSize(8 * 6, 9);
		findField("tonote").lock()->setSize(8 * 6, 9);
		findLabel("tonote").lock()->Hide(false);
		findLabel("tonote").lock()->setText("-");
		findField("tonote").lock()->Hide(false);
		findField("controlnumber").lock()->Hide(true);
	}
	else if (view == 3)
	{
		findLabel("fromnote").lock()->Hide(true);
		findField("fromnote").lock()->Hide(true);
		findLabel("tonote").lock()->Hide(true);
		findField("tonote").lock()->Hide(true);
		findField("controlnumber").lock()->Hide(false);
	}
	else if (view != 1 && view != 3)
	{
		findLabel("fromnote").lock()->Hide(true);
		findField("fromnote").lock()->Hide(true);
		findLabel("tonote").lock()->Hide(true);
		findField("tonote").lock()->Hide(true);
		findField("controlnumber").lock()->Hide(true);
	}
}

void StepEditorScreen::setViewNotesText()
{
	init();
	
	if (view == 1 && track.lock()->getBus() != 0)
	{
		if (fromNotePad != 34)
		{
			findField("fromnote").lock()->setText(to_string(fromNotePad) + "/" + sampler.lock()->getPadName(program.lock()->getPadIndexFromNote(fromNotePad)));
		}
		else
		{
			findField("fromnote").lock()->setText("ALL");
		}
	}
	else if (view == 1 && track.lock()->getBus() == 0)
	{
		findField("fromnote").lock()->setText(StrUtil::padLeft(to_string(noteA), " ", 3) + "(" + mpc::Util::noteNames()[noteA] + u8"\u00D4");
		findField("tonote").lock()->setText(StrUtil::padLeft(to_string(noteB), " ", 3) + "(" + mpc::Util::noteNames()[noteB] + u8"\u00D4");
	}
	else if (view == 3)
	{
		if (control == -1)
		{
			findField("controlnumber").lock()->setText("   -    ALL");
		}
		else
		{
			findField("controlnumber").lock()->setText(EventRow::controlNames[control]);
		}
	}

	findField("view").lock()->setText(viewNames[view]);
	auto newWidth = findField("view").lock()->getText().length() * 6 + 1;
	findField("view").lock()->setSize(newWidth, 9);
}

void StepEditorScreen::setView(int i)
{
	if (i < 0 || i > 7)
	{
		return;
	}

	view = i;
	refreshViewNotes();
	setViewNotesText();
	setyOffset(0);
}

void StepEditorScreen::setNoteA(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}

	noteA = i;
	
	if (noteA > noteB)
	{
		noteB = noteA;
	}

	setViewNotesText();
	displayView();
	refreshViewNotes();
	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setNoteB(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}
	
	noteB = i;
	
	if (noteB < noteA)
	{
		noteA = noteB;
	}

	setViewNotesText();
	displayView();
	refreshViewNotes();
	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setControl(int i)
{
	if (i < -1 || i > 127)
	{
		return;
	}

	control = i;
	
	setViewNotesText();
	displayView();
	refreshViewNotes();
	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setAutoStepIncrementEnabled(bool b)
{
	autoStepIncrementEnabled = b;
}

void StepEditorScreen::setDurationOfRecordedNotes(bool b)
{
	durationOfRecordedNotes = b;
}

void StepEditorScreen::setTcValueRecordedNotes(int i)
{
	if (i < 0 || i > 100)
	{
		return;
	}

	tcValueRecordedNotes = i;
}

void StepEditorScreen::setyOffset(int i)
{
	if (i < 0)
	{
		return;
	}

	yOffset = i;

	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setSelectedEventIndex(int i)
{
	selectedEventNumber = i;
}

void StepEditorScreen::setFromNotePad(int i)
{
	if (i < 34 || i > 98)
	{
		return;
	}
	fromNotePad = i;

	setViewNotesText();
	displayView();
	refreshViewNotes();
	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setSelectionStartIndex(int i)
{
	if (dynamic_pointer_cast<EmptyEvent>(eventsAtCurrentTick[i].lock()))
	{
		return;
	}

	selectionStartIndex = i;
	selectionEndIndex = i;
	
	ls.lock()->setFunctionKeysArrangement(1);
	refreshSelection();
}

void StepEditorScreen::clearSelection()
{
	selectionStartIndex = -1;
	selectionEndIndex = -1;
	ls.lock()->setFunctionKeysArrangement(0);
	refreshSelection();
}

void StepEditorScreen::setSelectionEndIndex(int i)
{
	if (i == -1)
	{
		return;
	}

	if (dynamic_pointer_cast<EmptyEvent>(eventsAtCurrentTick[i].lock()))
	{
		return;
	}
	selectionEndIndex = i;
	refreshSelection();
}

void StepEditorScreen::setSelectedEvents()
{
	selectedEvents.clear();
	auto firstEventIndex = selectionStartIndex;
	auto lastEventIndex = selectionEndIndex;
	
	if (firstEventIndex > lastEventIndex)
	{
		firstEventIndex = selectionEndIndex;
		lastEventIndex = selectionStartIndex;
	}
	
	for (int i = firstEventIndex; i < lastEventIndex + 1; i++)
	{
		selectedEvents.push_back(eventsAtCurrentTick[i]);
	}
}

void StepEditorScreen::checkSelection()
{
	string focus = ls.lock()->getFocus();

	if (focus.length() != 2)
	{
		clearSelection();
		return;
	}

	int eventNumber = stoi(focus.substr(1, 2));
	int  visibleEventCounter = 0;
	int firstSelectedVisibleEventIndex = -1;
	auto selectedEventCounter = 0;

	for (auto& EventRowParameters : findEventRowParameterss())
	{
		if (!EventRowParameters.lock()->IsHidden())
		{
			if (firstSelectedVisibleEventIndex == -1)
			{
				firstSelectedVisibleEventIndex = visibleEventCounter;
			}
			selectedEventCounter++;
		}
		visibleEventCounter++;
	}

	if (firstSelectedVisibleEventIndex != -1)
	{
		int lastSelectedVisibleEventIndex = firstSelectedVisibleEventIndex + selectedEventCounter - 1;

		if (!dynamic_pointer_cast<EmptyEvent>(visibleEvents[eventNumber].lock()))
		{
			if (eventNumber < firstSelectedVisibleEventIndex || eventNumber > lastSelectedVisibleEventIndex)
			{
				clearSelection();
			}
		}
	}
}

void StepEditorScreen::setSelectedEvent(weak_ptr<Event> event)
{
	selectedEvent = event;
}

void StepEditorScreen::setSelectedParameterLetter(string str)
{
	selectedParameterLetter = str;
}

void StepEditorScreen::setChangeNoteToIndex(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}
	changeNoteToNumber = i;
	// Should call 		updateEditMultiple(); in StepWindowObserver
}

void StepEditorScreen::setChangeVariationTypeNumber(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}
	changeVariationTypeNumber = i;
	// Should call 		updateEditMultiple(); in StepWindowObserver
}

void StepEditorScreen::setChangeVariationValue(int i)
{
	if (i < 0 || i > 128)
	{
		return;
	}
	
	if (changeVariationTypeNumber != 0 && i > 100)
	{
		i = 100;
	}
	
	changeVariationValue = i;
	// Should call 		updateEditMultiple(); in StepWindowObserver
}

void StepEditorScreen::setEditValue(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}

	editValue = i;
	// Should call 		updateEditMultiple(); in StepWindowObserver
}

void StepEditorScreen::removeEvents()
{
	init();
	auto firstEventIndex = selectionStartIndex;
	auto lastEventIndex = selectionEndIndex;
	
	if (firstEventIndex > lastEventIndex)
	{
		firstEventIndex = selectionEndIndex;
		lastEventIndex = selectionStartIndex;
	}
	
	int eventCounter = 0;
	
	vector<weak_ptr<Event>> tempList;
	
	for (auto& e : eventsAtCurrentTick)
	{
		tempList.push_back(e);
	}
	
	for (auto& event : tempList)
	{
		if (eventCounter >= firstEventIndex && eventCounter <= lastEventIndex)
		{
			if (!dynamic_pointer_cast<EmptyEvent>(event.lock()))
			{
				track.lock()->removeEvent(event);
			}
		}
		eventCounter++;
	}
	clearSelection();
	setyOffset(0);
}

void StepEditorScreen::displayView()
{
	findField("view").lock()->setText(viewNames[view]);
}

void StepEditorScreen::update(moduru::observer::Observable*, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("step-editor") == 0)
	{
		if (mpc.getControls().lock()->getPressedPads()->size() != 0)
		{
			// a note is currently being recorded by the user pressing a pad
			initVisibleEvents();
			refreshEventRows();
			return;
		}

		int eventNumber;

		try
		{
			init();
			eventNumber = stoi(param.substr(1, 2));
		}
		catch (const std::invalid_argument& ia)
		{
			return;
		}

		auto eventRow = findEventRows()[eventNumber].lock();

		if (dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber].lock()))
		{
			if (track.lock()->getBus() != 0)
			{
				eventRow->setDrumNoteEventValues();
			}
			else
			{
				eventRow->setMidiNoteEventValues();
			}
		}
		else if (dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setMixerEventValues();
		}
		else if (dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber].lock())
			|| dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setMiscEventValues();
		}
		else if (dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setControlChangeEventValues();
		}
		else if (dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setChannelPressureEventValues();
		}
		else if (dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setPolyPressureEventValues();
		}
		else if (dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setSystemExclusiveEventValues();
		}
		else if (dynamic_pointer_cast<EmptyEvent>(visibleEvents[eventNumber].lock()))
		{
			eventRow->setEmptyEventValues();
		}
	}
	else if (msg.compare("adjust-duration") == 0)
	{
		initVisibleEvents();
		refreshEventRows();
	}
	else if (msg.compare("bar") == 0)
	{
		if (sequencer.lock()->isPlaying())
		{
			return;
		}
		findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
		setyOffset(0);
	}
	else if (msg.compare("beat") == 0)
	{
		if (sequencer.lock()->isPlaying())
		{
			return;
		}
		findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
		setyOffset(0);
	}
	else if (msg.compare("clock") == 0)
	{
		if (sequencer.lock()->isPlaying())
		{
			return;
		}
		findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
		setyOffset(0);
	}
}

vector<weak_ptr<Event>>& StepEditorScreen::getVisibleEvents()
{
	return visibleEvents;
}

vector<weak_ptr<Event>>& StepEditorScreen::getSelectedEvents()
{
	return selectedEvents;
}

string StepEditorScreen::getSelectedParameterLetter()
{
	return selectedParameterLetter;
}

weak_ptr<Event> StepEditorScreen::getSelectedEvent()
{
	return selectedEvent;
}

int StepEditorScreen::getChangeVariationTypeNumber()
{
	return changeVariationTypeNumber;
}

int StepEditorScreen::getChangeVariationValue()
{
	return changeVariationValue;
}

int StepEditorScreen::getChangeNoteToNumber()
{
	return changeNoteToNumber;
}

int StepEditorScreen::getEditValue()
{
	return editValue;
}

vector<weak_ptr<Event>>& StepEditorScreen::getPlaceHolder()
{
	return placeHolder;
}

int StepEditorScreen::getYOffset()
{
	return yOffset;
}

bool StepEditorScreen::isAutoStepIncrementEnabled()
{
	return autoStepIncrementEnabled;
}

bool StepEditorScreen::isDurationTcPercentageEnabled()
{
	return durationTcPercentageEnabled;
}

int StepEditorScreen::getTcValueRecordedNotes()
{
	return tcValueRecordedNotes;
}

vector<weak_ptr<EventRowParameters>> StepEditorScreen::findEventRowParameterss()
{
	vector<weak_ptr<EventRowParameters>> result;

	for (auto& eventRow : findEventRows())
	{
		auto child = eventRow.lock()->findChild("event-row-parameters").lock();
		{
			auto candidate = dynamic_pointer_cast<EventRowParameters>(child);
			
			if (candidate)
			{
				result.push_back(candidate);
			}
		}
	}

	return result;
}

vector<weak_ptr<EventRow>> StepEditorScreen::findEventRows()
{
	vector<weak_ptr<EventRow>> result;

	for (int i = 0; i < 4; i++)
	{
		result.push_back(dynamic_pointer_cast<EventRow>(findChild("event-row-" + to_string(i)).lock()));
	}

	return result;
}
