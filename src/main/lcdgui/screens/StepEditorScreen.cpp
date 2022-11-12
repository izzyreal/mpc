#include "StepEditorScreen.hpp"

#include <audiomidi/EventHandler.hpp>
#include <audiomidi/AudioMidiServices.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/EditMultipleScreen.hpp>

#include <lcdgui/Rectangle.hpp>

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <Util.hpp>

#include <mpc/MpcMultiMidiSynth.hpp>
#include <midi/core/ShortMessage.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace moduru::lang;

const int EVENT_ROW_COUNT = 4;

StepEditorScreen::StepEditorScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "step-editor", layerIndex)
{
	lastColumn["empty"] = "a";
	lastColumn["channel-pressure"] = "a";
	lastColumn["control-change"] = "a";
	lastColumn["midi-clock"] = "a";
	lastColumn["mixer"] = "a";
	lastColumn["note"] = "a";
	lastColumn["pitch-bend"] = "a";
	lastColumn["poly-pressure"] = "a";
	lastColumn["program-change"] = "a";
	lastColumn["system-exclusive"] = "a";
	lastColumn["tempo-change"] = "a";

	for (int i = 0; i < EVENT_ROW_COUNT; i++)
		addChildT<EventRow>(mpc, i);

	MRECT r(31, 0, 164, 9);
	addChildT<Rectangle>("view-background", r);
}

void StepEditorScreen::openWindow()
{
    openScreen("step-edit-options");
}


void StepEditorScreen::open()
{
	findField("tonote")->setLocation(115, 0);
	findLabel("fromnote")->Hide(true);
	auto rectangle = findChild<Rectangle>("view-background");

	lastRow = 0;

	init();

	if (track.lock()->getBus() != 0)
	{
		int pgm = sampler->getDrumBusProgramIndex(track.lock()->getBus());
		program = sampler->getProgram(pgm);
		findField("fromnote")->setAlignment(Alignment::None);
	}
	else
	{
		findField("fromnote")->setAlignment(Alignment::Centered, 18);
		findField("tonote")->setAlignment(Alignment::Centered, 18);

		if (lastColumn["note"] == "e")
			lastColumn["note"] = "c";
	}

	updateComponents();
	setViewNotesText();
	displayView();
	sequencer->addObserver(this);
	track.lock()->addObserver(this);

	findField("now0")->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
	findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
	findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");

	initVisibleEvents();

	refreshEventRows();
	refreshSelection();

	auto previousScreen = ls->getPreviousScreenName();

	if (previousScreen != "step-timing-correct" &&
		previousScreen != "insert-event" &&
		previousScreen != "paste-event")
	{
		auto eventType = visibleEvents[0]->getTypeName();
		ls->setFocus(lastColumn[eventType] + "0");
	}
}

void StepEditorScreen::close()
{
	init();
	sequencer->deleteObserver(this);
	track.lock()->deleteObserver(this);

	if (param.length() == 2)
	{
		int srcNumber = stoi(param.substr(1, 2));
		auto srcLetter = param.substr(0, 1);
		auto eventType = visibleEvents[srcNumber]->getTypeName();
		lastColumn[eventType] = srcLetter;
	}

	auto nextScreen = ls->getCurrentScreenName();

	if (nextScreen != "step-timing-correct" &&
		nextScreen != "insert-event" &&
		nextScreen != "paste-event")
	{
		track.lock()->removeDoubles();
	}

	for (auto& e : visibleEvents)
	{
		if (e) e->deleteObserver(this);
	}

	for (auto& e : eventsAtCurrentTick)
    {
        if (e) e->deleteObserver(this);
    }

	clearSelection();
}

void StepEditorScreen::function(int i)
{
	init();

	switch (i)
	{
	case 0:
		openScreen("step-timing-correct");
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
			auto eventIndex = stoi(param.substr(1, 1));
			auto maybeEmptyEvent = std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[eventIndex]);

			if (!maybeEmptyEvent)
				placeHolder = { visibleEvents[eventIndex] };
		}
		break;
	case 2:
	{
		if (param.length() != 2)
			return;

		auto rowIndex = stoi(param.substr(1, 2));

		if (selectionStartIndex != -1)
		{
			removeEvents();
			ls->setFocus("a0");
			return;
		}

		if (!std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[rowIndex]))
		{
			for (int e = 0; e < track.lock()->getEvents().size(); e++)
			{
				if (track.lock()->getEvents()[e] == visibleEvents[rowIndex])
				{
					track.lock()->removeEvent(e);
					break;
				}
			}

			if (rowIndex == 2 && yOffset > 0)
				yOffset--;
		}

		initVisibleEvents();
		refreshEventRows();
		refreshSelection();

		auto eventType = visibleEvents[rowIndex]->getTypeName();

		ls->setFocus(lastColumn[eventType] + std::to_string(rowIndex));
		break;
	}
	case 3:
	{
		bool posIsLastTick = sequencer->getTickPosition() == sequencer->getActiveSequence()->getLastTick();

		if (selectionEndIndex == -1)
		{
			if (!posIsLastTick)
				openScreen("insert-event");
		}
		else
		{
			auto row = stoi(param.substr(1, 2));

			auto event = visibleEvents[row];

			auto pitchEvent = std::dynamic_pointer_cast<PitchBendEvent>(event);
			auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
			auto sysexEvent = std::dynamic_pointer_cast<SystemExclusiveEvent>(event);
			auto maybeEmptyEvent = std::dynamic_pointer_cast<EmptyEvent>(event);

			if (pitchEvent || mixerEvent || sysexEvent || maybeEmptyEvent)
				return;

			auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(event);
			auto pgmChangeEvent = std::dynamic_pointer_cast<ProgramChangeEvent>(event);
			auto chPressEvent = std::dynamic_pointer_cast<ChannelPressureEvent>(event);
			auto polyPressEvent = std::dynamic_pointer_cast<PolyPressureEvent>(event);
			auto controlChangeEvent = std::dynamic_pointer_cast<ControlChangeEvent>(event);

			auto column = param.substr(0, 1);

			bool isA = column == "a";
			bool isB = column == "b";
			bool isC = column == "c";
			bool isD = column == "d";
			bool isE = column == "e";

			if ((polyPressEvent || controlChangeEvent) && isA)
				return;

			auto editMultipleScreen = mpc.screens->get<EditMultipleScreen>("edit-multiple");

			if (noteEvent && track.lock()->getBus() != 0)
			{
				if (isA)
				{
					editMultipleScreen->setChangeNoteTo(noteEvent->getNote());
				}
				else if (isB)
				{
					editMultipleScreen->setVariationType(noteEvent->getVariationType());
				}
				else if (isC)
				{
					editMultipleScreen->setVariationType(noteEvent->getVariationType());
					editMultipleScreen->seVariationValue(noteEvent->getVariationValue());
				}
				else if (isD)
				{
					editMultipleScreen->setEditValue(noteEvent->getDuration());
				}
				else if (isE)
				{
					editMultipleScreen->setEditValue(noteEvent->getVelocity());
				}
			}

			if (noteEvent && track.lock()->getBus() == 0)
			{
				if (isA)
					editMultipleScreen->setChangeNoteTo(noteEvent->getNote());
				else if (isB)
					editMultipleScreen->setEditValue(noteEvent->getDuration());
				else if (isC)
					editMultipleScreen->setEditValue(noteEvent->getVelocity());
			}
			else if (pgmChangeEvent)
			{
				editMultipleScreen->setEditValue(0);
			}
			else if (chPressEvent)
			{
				editMultipleScreen->setEditValue(chPressEvent->getAmount());
			}
			else if (polyPressEvent)
			{
				editMultipleScreen->setEditValue(polyPressEvent->getAmount());
			}
			else if (controlChangeEvent)
			{
				editMultipleScreen->setEditValue(controlChangeEvent->getAmount());
			}

			setSelectedEvent(visibleEvents[row]);
			setSelectedEvents();
			setSelectedParameterLetter(column);
			openScreen("edit-multiple");
		}

		break;
	}
	case 4:
		if (placeHolder.size() != 0)
			openScreen("paste-event");
		break;
	case 5:
		if (selectionStartIndex == -1)
		{
			if (param.length() == 2)
			{
				auto eventNumber = stoi(param.substr(1, 2));
				auto event = visibleEvents[eventNumber];
				auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(event);

				if (noteEvent)
                {
                    adhocPlayNoteEvent(noteEvent);
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

	if (param == "view")
	{
		setView(view + i);
	}
	else if (param == "now0")
	{
		sequencer->setBar(sequencer->getCurrentBarIndex() + i);
    }
	else if (param == "now1")
	{
		sequencer->setBeat(sequencer->getCurrentBeatIndex() + i);
	}
	else if (param == "now2")
	{
		sequencer->setClock(sequencer->getCurrentClockNumber() + i);
	}
	else if (param == "tcvalue")
	{
		auto screen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
		auto noteValue = screen->getNoteValue();
		screen->setNoteValue(noteValue + i);
	}
	else if (param == "fromnote" && view == 1)
	{
		if (track.lock()->getBus() != 0) setFromNote(fromNote + i);
		if (track.lock()->getBus() == 0) setNoteA(noteA + i);
	}
	else if (param == "tonote")
	{
		setNoteB(noteB + i);
	}
	else if (param == "fromnote" && view == 3)
	{
		setControl(control + i);
	}
	else if (param.length() == 2)
	{
		auto eventNumber = stoi(param.substr(1, 2));

		auto sysEx = std::dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber]);
		auto channelPressure = std::dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber]);
		auto polyPressure = std::dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber]);
		auto controlChange = std::dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber]);
		auto programChange = std::dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber]);
		auto pitchBend = std::dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber]);
		auto mixer = std::dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber]);
		auto note = std::dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber]);

		if (sysEx)
		{
			if (param.find("a") != std::string::npos)
				sysEx->setByteA(sysEx->getByteA() + i);
			else if (param.find("b") != std::string::npos)
				sysEx->setByteB(sysEx->getByteB() + i);
		}
		else if (channelPressure)
		{
			if (param.find("a") != std::string::npos)
				channelPressure->setAmount(channelPressure->getAmount() + i);
		}
		else if (polyPressure)
		{
			if (param.find("a") != std::string::npos)
				polyPressure->setNote(polyPressure->getNote() + i);
			else if (param.find("b") != std::string::npos)
				polyPressure->setAmount(polyPressure->getAmount() + i);
		}
		else if (controlChange)
		{
			if (param.find("a") != std::string::npos)
				controlChange->setController(controlChange->getController() + i);
			else if (param.find("b") != std::string::npos)
				controlChange->setAmount(controlChange->getAmount() + i);
		}
		else if (programChange)
		{
			if (param.find("a") != std::string::npos)
				programChange->setProgram(programChange->getProgram() + i);
		}
		else if (pitchBend)
		{
			if (param.find("a") != std::string::npos)
				pitchBend->setAmount(pitchBend->getAmount() + i);
		}
		else if (mixer)
		{
			if (param.find("a") != std::string::npos)
				mixer->setParameter(mixer->getParameter() + i);
			else if (param.find("b") != std::string::npos)
				mixer->setPadNumber(mixer->getPad() + i);
			else if (param.find("c") != std::string::npos)
				mixer->setValue(mixer->getValue() + i);
		}
		else if (note && track.lock()->getBus() == 0)
		{
			if (param.find("a") != std::string::npos)
				note->setNote(note->getNote() + i);
			else if (param.find("b") != std::string::npos)
				note->setDuration(note->getDuration() + i);
			else if (param.find("c") != std::string::npos)
				note->setVelocity(note->getVelocity() + i);
		}
		else if (note && track.lock()->getBus() != 0)
		{
			if (param.find("a") != std::string::npos)
			{
				if (note->getNote() + i > 98)
				{
					if (note->getNote() != 98)
						note->setNote(98);

					return;
				}
				else if (note->getNote() + i < 35)
				{
					if (note->getNote() != 35)
						note->setNote(35);

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
			else if (param.find("b") != std::string::npos)
			{
				note->setVariationTypeNumber(note->getVariationType() + i);
			}
			else if (param.find("c") != std::string::npos)
			{
				note->setVariationValue(note->getVariationValue() + i);
			}
			else if (param.find("d") != std::string::npos)
			{
				note->setDuration(note->getDuration() + i);
			}
			else if (param.find("e") != std::string::npos)
			{
				note->setVelocity(note->getVelocity() + i);
			}
		}
	}

    if (param.find("now") != std::string::npos)
    {
        adhocPlayNoteEventsAtCurrentPosition();
    }

	refreshSelection();
}

void StepEditorScreen::prevStepEvent()
{
	init();

    track.lock()->removeDoubles();

    auto controls = mpc.getControls();

	if (controls->isGoToPressed())
		sequencer->goToPreviousEvent();
	else
		sequencer->goToPreviousStep();

    adhocPlayNoteEventsAtCurrentPosition();
}

void StepEditorScreen::nextStepEvent()
{
	init();

    track.lock()->removeDoubles();

    auto controls = mpc.getControls();

	if (controls->isGoToPressed())
		sequencer->goToNextEvent();
	else
		sequencer->goToNextStep();

    adhocPlayNoteEventsAtCurrentPosition();
}

void StepEditorScreen::prevBarStart()
{
	init();

    track.lock()->removeDoubles();

    auto controls = mpc.getControls();

	if (controls->isGoToPressed())
		sequencer->setBar(0);
	else
		sequencer->setBar(sequencer->getCurrentBarIndex() - 1);

    adhocPlayNoteEventsAtCurrentPosition();
}

void StepEditorScreen::nextBarEnd()
{
	init();

    track.lock()->removeDoubles();

    auto controls = mpc.getControls();

	if (controls->isGoToPressed())
		sequencer->setBar(sequencer->getActiveSequence()->getLastBarIndex() + 1);
	else
		sequencer->setBar(sequencer->getCurrentBarIndex() + 1);

    adhocPlayNoteEventsAtCurrentPosition();
}

void StepEditorScreen::left()
{
	init();

	if (param.length() == 2 && param.substr(0, 1) == "a")
	{
		lastRow = stoi(param.substr(1, 2));
		ls->setFocus("view");
	}
	else
	{
		ScreenComponent::left();
	}

	checkSelection();
	refreshSelection();
}

void StepEditorScreen::right()
{
	ScreenComponent::right();
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
		auto controls = mpc.getControls();

		if (controls->isShiftPressed() && selectionStartIndex == -1 && std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[srcNumber]))
			return;

		if (!controls->isShiftPressed() && srcNumber == 0 && yOffset == 0)
		{
			clearSelection();
			auto eventType = visibleEvents[srcNumber]->getTypeName();
			lastColumn[eventType] = srcLetter;
			lastRow = 0;
			ls->setFocus("view");
			refreshSelection();
			return;
		}

		if (srcNumber == 0 && yOffset != 0)
		{
			auto oldEventType = visibleEvents[srcNumber]->getTypeName();
			lastColumn[oldEventType] = srcLetter;

			setyOffset(yOffset - 1);

			auto newEventType = visibleEvents[srcNumber]->getTypeName();

			ls->setFocus(lastColumn[newEventType] + std::to_string(srcNumber));

			if (controls->isShiftPressed())
				setSelectionEndIndex(srcNumber + yOffset);

			refreshSelection();
			return;
		}

		downOrUp(-1);
	}
}

void StepEditorScreen::down()
{
	init();

	if (param == "view" ||
        param.find("now") != std::string::npos ||
        param == "fromnote" ||
        param == "tonote")
	{
		auto eventType = visibleEvents[lastRow]->getTypeName();
		ls->setFocus(lastColumn[eventType] + std::to_string(lastRow));
		return;
	}

	if (param.length() == 2)
	{
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto controls = mpc.getControls();

		if (srcNumber == 3)
		{
			if (yOffset + 4 == eventsAtCurrentTick.size())
			{
				return;
			}

			auto oldEventType = visibleEvents[srcNumber]->getTypeName();
			lastColumn[oldEventType] = srcLetter;

			setyOffset(yOffset + 1);

			auto newEventType = visibleEvents[srcNumber]->getTypeName();
			auto newColumn = lastColumn[newEventType];

			ls->setFocus(newColumn + "3");

			if (controls->isShiftPressed() && std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[3]))
				setSelectionEndIndex(srcNumber + yOffset);

			refreshSelection();
			return;
		}

		downOrUp(1);
	}
}

void StepEditorScreen::shift()
{
	init();
	ScreenComponent::shift();

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
		auto controls = mpc.getControls();
		auto destination = srcLetter + std::to_string(srcNumber + increment);

		if (srcNumber + increment != -1)
		{
			if (visibleEvents[srcNumber + increment])
			{
				auto oldEventType = visibleEvents[srcNumber]->getTypeName();
				lastColumn[oldEventType] = srcLetter;

				auto newEventType = visibleEvents[srcNumber + increment]->getTypeName();
				ls->setFocus(lastColumn[newEventType] + std::to_string(srcNumber + increment));
			}
		}

		if (controls->isShiftPressed())
		{
			setSelectionEndIndex(srcNumber + increment + yOffset);
		}
		else
		{
			checkSelection();
			refreshSelection();
		}
	}
}

void StepEditorScreen::refreshSelection()
{
	auto firstEventIndex = std::min(selectionStartIndex, selectionEndIndex);
	auto lastEventIndex = std::max(selectionStartIndex, selectionEndIndex);

	bool somethingSelected = false;

	if (firstEventIndex != -1)
	{
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
		ls->setFunctionKeysArrangement(1);
	}
}

void StepEditorScreen::initVisibleEvents()
{
	init();

	for (auto& e : eventsAtCurrentTick)
		if (e) e->deleteObserver(this);

	eventsAtCurrentTick.clear();

	for (auto& event : track.lock()->getEvents())
	{
		if (event->getTick() == sequencer->getTickPosition())
		{
			if ((view == 0
				|| view == 1)
				&& std::dynamic_pointer_cast<NoteEvent>(event))
			{
				auto ne = std::dynamic_pointer_cast<NoteEvent>(event);

				if (track.lock()->getBus() != 0)
				{
					if (fromNote == 34 || view == 0)
					{
						eventsAtCurrentTick.push_back(ne);
					}
					else if (fromNote != 34
						&& fromNote == ne->getNote())
					{
						eventsAtCurrentTick.push_back(ne);
					}
				}
				else
				{
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
				&& std::dynamic_pointer_cast<PitchBendEvent>(event))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0 || view == 3)
				&& std::dynamic_pointer_cast<ControlChangeEvent>(event))
			{
				if (control == -1) {
					eventsAtCurrentTick.push_back(event);
				}
				if (control == std::dynamic_pointer_cast<ControlChangeEvent>(event)->getController())
				{
					eventsAtCurrentTick.push_back(event);
				}
			}
			if ((view == 0
				|| view == 4)
				&& std::dynamic_pointer_cast<ProgramChangeEvent>(event))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0
				|| view == 5)
				&& std::dynamic_pointer_cast<ChannelPressureEvent>(event))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0
				|| view == 6)
				&& std::dynamic_pointer_cast<PolyPressureEvent>(event))
			{
				eventsAtCurrentTick.push_back(event);
			}

			if ((view == 0
				|| view == 7)
				&& (std::dynamic_pointer_cast<SystemExclusiveEvent>(event)
					|| std::dynamic_pointer_cast<MixerEvent>(event)))
			{
				eventsAtCurrentTick.push_back(event);
			}

		}
	}

	eventsAtCurrentTick.push_back(emptyEvent);

	for (auto& e : visibleEvents)
	{
		if (e) e->deleteObserver(this);
	}

	visibleEvents = std::vector<std::shared_ptr<Event>>(4);
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
		auto event = visibleEvents[i];

		if (event)
		{
			eventRow->Hide(false);
			event->addObserver(this);
			eventRow->setBus(sequencer->getActiveTrack()->getBus());
		}
		else
		{
			eventRow->Hide(true);
		}

		eventRow->setEvent(event);
		eventRow->init();
	}
}

void StepEditorScreen::updateComponents()
{
	init();

	if (view == 1 && track.lock()->getBus() != 0)
	{
		findField("fromnote")->Hide(false);
		findField("fromnote")->setSize(37, 9);
		findField("fromnote")->setLocation(67, 0);
		findLabel("tonote")->Hide(true);
		findField("tonote")->Hide(true);
	}
	else if (view == 1 && track.lock()->getBus() == 0)
	{
		findField("fromnote")->Hide(false);
		findField("fromnote")->setLocation(61, 0);
		findField("fromnote")->setSize(47, 9);
		findField("tonote")->setSize(47, 9);
		findLabel("tonote")->Hide(false);
		findLabel("tonote")->setText("-");
		findField("tonote")->Hide(false);
	}
	else if (view == 3)
	{
		auto fromNoteField = findField("fromnote");
		fromNoteField->Hide(false);
		fromNoteField->setLocation(60, 0);
		fromNoteField->setSize(104, 9);

		findLabel("tonote")->Hide(true);
		findField("tonote")->Hide(true);
	}
	else if (view != 1 && view != 3)
	{
		findField("fromnote")->Hide(true);
		findLabel("tonote")->Hide(true);
		findField("tonote")->Hide(true);
	}
}

void StepEditorScreen::setViewNotesText()
{
	init();

	if (view == 1 && track.lock()->getBus() != 0)
	{
		if (fromNote == 34) {
            findField("fromnote")->setText("ALL");
        }
		else
        {
            auto padName = sampler->getPadName(program.lock()->getPadIndexFromNote(fromNote));
			findField("fromnote")->setText(std::to_string(fromNote) + "/" + padName);
        }
	}
	else if (view == 1 && track.lock()->getBus() == 0)
	{
		findField("fromnote")->setText(StrUtil::padLeft(std::to_string(noteA), " ", 3) + "(" + mpc::Util::noteNames()[noteA] + u8"\u00D4");
		findField("tonote")->setText(StrUtil::padLeft(std::to_string(noteB), " ", 3) + "(" + mpc::Util::noteNames()[noteB] + u8"\u00D4");
	}
	else if (view == 3)
	{
		if (control == -1)
			findField("fromnote")->setText("   -    ALL");
		else
			findField("fromnote")->setText(StrUtil::padLeft(std::to_string(control), " ", 3) + "-" + EventRow::controlNames[control]);
	}

	findField("view")->setText(viewNames[view]);
	auto newWidth = findField("view")->getText().length() * 6 + 1;
	findField("view")->setSize(newWidth, 9);
}

void StepEditorScreen::setView(int i)
{
	if (i < 0 || i > 7)
		return;

	view = i;

	displayView();
	updateComponents();
	setViewNotesText();
	setyOffset(0);
	findChild<Rectangle>()->SetDirty();
}

void StepEditorScreen::setNoteA(int i)
{
	if (i < 0 || i > 127)
		return;

	noteA = i;

	if (noteA > noteB)
		noteB = noteA;

	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setNoteB(int i)
{
	if (i < 0 || i > 127)
		return;

	noteB = i;

	if (noteB < noteA)
		noteA = noteB;

	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setControl(int i)
{
	if (i < -1 || i > 127)
		return;

	control = i;

	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setyOffset(int i)
{
	if (i < 0)
		return;

	yOffset = i;

	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setFromNote(int i)
{
	if (i < 34 || i > 98)
		return;

	fromNote = i;

	setViewNotesText();
	displayView();
	updateComponents();
	setViewNotesText();
	initVisibleEvents();
	refreshEventRows();
	refreshSelection();
}

void StepEditorScreen::setSelectionStartIndex(int i)
{
	if (std::dynamic_pointer_cast<EmptyEvent>(eventsAtCurrentTick[i]))
		return;

	selectionStartIndex = i;
	selectionEndIndex = i;

	ls->setFunctionKeysArrangement(1);
	refreshSelection();
}

void StepEditorScreen::clearSelection()
{
	selectionStartIndex = -1;
	selectionEndIndex = -1;
	ls->setFunctionKeysArrangement(0);
	refreshSelection();
}

void StepEditorScreen::setSelectionEndIndex(int i)
{
	if (i == -1)
		return;

	selectionEndIndex = i;
	refreshSelection();
}

void StepEditorScreen::finalizeSelection(int i)
{
	selectionEndIndex = i;

	if (selectionEndIndex < selectionStartIndex)
	{
		auto oldSelectionStartIndex = selectionStartIndex;
		selectionStartIndex = selectionEndIndex;
		selectionEndIndex = oldSelectionStartIndex;
	}

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
		selectedEvents.push_back(eventsAtCurrentTick[i]);
}

void StepEditorScreen::checkSelection()
{
    std::string focus = ls->getFocus();

	if (focus.length() != 2)
	{
		clearSelection();
		return;
	}

	int row = stoi(focus.substr(1, 2));
	int eventIndex = row + yOffset;

	if (eventIndex < selectionStartIndex || eventIndex > selectionEndIndex)
		clearSelection();
}

void StepEditorScreen::setSelectedEvent(std::weak_ptr<Event> event)
{
	selectedEvent = event.lock();
}

void StepEditorScreen::setSelectedParameterLetter(std::string str)
{
	selectedParameterLetter = str;
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

	std::vector<std::shared_ptr<Event>> tempList;

	for (auto& e : eventsAtCurrentTick)
		tempList.push_back(e);

	for (auto& event : tempList)
	{
		if (eventCounter >= firstEventIndex && eventCounter <= lastEventIndex)
		{
			if (!std::dynamic_pointer_cast<EmptyEvent>(event))
				track.lock()->removeEvent(event);
		}
		eventCounter++;
	}

	clearSelection();
	setyOffset(0);
}

void StepEditorScreen::displayView()
{
	findField("view")->setText(viewNames[view]);
}

void StepEditorScreen::update(moduru::observer::Observable*, nonstd::any message)
{
	auto msg = nonstd::any_cast<std::string>(message);

    if (msg == "step-editor")
	{
        auto& pads = mpc.getHardware()->getPads();

        auto anyPadIsPressed = std::any_of(
                pads.begin(),
                pads.end(),
                [](const std::shared_ptr<mpc::hardware::HwPad> &p) {
                    return p->isPressed();
                });

        if (anyPadIsPressed)
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
		catch (const std::invalid_argument& e)
		{
			return;
		}

		auto eventRow = findEventRows()[eventNumber].lock();

		if (std::dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber]))
		{
			if (track.lock()->getBus() != 0)
				eventRow->setDrumNoteEventValues();
			else
				eventRow->setMidiNoteEventValues();
		}
		else if (std::dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setMixerEventValues();
		}
		else if (std::dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber])
			|| std::dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setMiscEventValues();
		}
		else if (std::dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setControlChangeEventValues();
		}
		else if (std::dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setChannelPressureEventValues();
		}
		else if (std::dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setPolyPressureEventValues();
		}
		else if (std::dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setSystemExclusiveEventValues();
		}
		else if (std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[eventNumber]))
		{
			eventRow->setEmptyEventValues();
		}
	}
	else if (msg == "adjust-duration")
	{
		initVisibleEvents();
		refreshEventRows();
	}
	else if (msg == "bar")
	{
		if (sequencer->isPlaying())
			return;

		findField("now0")->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
		setyOffset(0);
	}
	else if (msg == "beat")
	{
		if (sequencer->isPlaying())
			return;

		findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
		setyOffset(0);
	}
	else if (msg == "clock")
	{
		if (sequencer->isPlaying())
			return;

		findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");
		setyOffset(0);
	}
}

std::vector<std::shared_ptr<Event>>& StepEditorScreen::getVisibleEvents()
{
	return visibleEvents;
}

std::vector<std::shared_ptr<Event>>& StepEditorScreen::getSelectedEvents()
{
	return selectedEvents;
}

std::string StepEditorScreen::getSelectedParameterLetter()
{
	return selectedParameterLetter;
}

std::shared_ptr<Event> StepEditorScreen::getSelectedEvent()
{
	return selectedEvent;
}

std::vector<std::shared_ptr<Event>>& StepEditorScreen::getPlaceHolder()
{
	return placeHolder;
}

int StepEditorScreen::getYOffset()
{
	return yOffset;
}

std::vector<std::weak_ptr<EventRow>> StepEditorScreen::findEventRows()
{
	std::vector<std::weak_ptr<EventRow>> result;

	for (int i = 0; i < 4; i++)
		result.push_back(std::dynamic_pointer_cast<EventRow>(findChild("event-row-" + std::to_string(i))));

	return result;
}

void StepEditorScreen::adhocPlayNoteEvent(const std::shared_ptr<mpc::sequencer::NoteEvent> &noteEvent)
{
    auto tick = noteEvent->getTick();
    noteEvent->setTick(-1);
    auto eventHandler = mpc.getEventHandler();
    auto tr = track.lock().get();

    MidiAdapter midiAdapter;

    auto mms = mpc.getMms();

    midiAdapter.process(noteEvent, tr->getBus() - 1, noteEvent ? noteEvent->getVelocity() : 0);

    auto varType = noteEvent->getVariationType();
    auto varValue = noteEvent->getVariationValue();

    int uniqueEnoughID = playSingleEventCounter++;

    if (playSingleEventCounter < 0) playSingleEventCounter = 0;

    mms->mpcTransport(midiAdapter.get().lock().get(), 0, varType, varValue, 0, uniqueEnoughID, -1);

    noteEvent->setTick(tick);

    auto frameSeq = mpc.getAudioMidiServices()->getFrameSequencer();
    auto sampleRate = mpc.getAudioMidiServices()->getAudioServer()->getSampleRate();
    auto tempo = sequencer->getTempo();
    auto &events = frameSeq->eventsAfterNFrames;

    for (auto &e: events)
    {
        if (!e.occupied.load())
        {
            auto durationInFrames = mpc::sequencer::SeqUtil::ticksToFrames(noteEvent->getDuration(),
                                                                           tempo, sampleRate);
            e.init(durationInFrames, [noteEvent, tr, mms, uniqueEnoughID]() {
                auto noteOff = noteEvent->getNoteOff().lock();
                auto noteOffTick = noteOff->getTick();
                noteOff->setTick(-1);
                noteOff->setNote(noteEvent->getNote());
                MidiAdapter midiAdapter2;
                midiAdapter2.process(noteOff, tr->getBus() - 1, 0);
                auto noteOffToSend = midiAdapter2.get();
                noteOff->setTick(noteOffTick);
                mms->mpcTransport(noteOffToSend.lock().get(), 0, 0, 0, 0, uniqueEnoughID, -1);
            });
            break;
        }
    }
}

void StepEditorScreen::adhocPlayNoteEventsAtCurrentPosition()
{
    auto tick = sequencer->getTickPosition();
    for (auto& e : track.lock()->getEventRange(tick, tick))
    {
        auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(e);
        if (noteEvent)
        {
            adhocPlayNoteEvent(noteEvent);
        }
    }
}
