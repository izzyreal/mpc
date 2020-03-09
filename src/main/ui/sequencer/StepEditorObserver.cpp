#include "StepEditorObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>

#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequencer.hpp>
#include <observer/Observable.hpp>

#include <lang/StrUtil.hpp>

#include <typeinfo>

using namespace mpc::ui::sequencer;
using namespace mpc::sequencer;
using namespace std;

StepEditorObserver::StepEditorObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	emptyEvent = make_shared<EmptyEvent>();
	viewNames = { "ALL EVENTS", "NOTES", "PITCH BEND", "CTRL:", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE" };
	stepEditorGui = mpc->getUis().lock()->getStepEditorGui();
	stepEditorGui->addObserver(this);
	sequencer = mpc->getSequencer();
	sampler = mpc->getSampler();
	auto lSequencer = sequencer.lock();
	auto seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	auto seq = sequence.lock();
	track = seq->getTrack(lSequencer->getActiveTrackIndex());
	auto lTrk = track.lock();
	auto ls = mpc->getLayeredScreen().lock();
	viewField = ls->lookupField("viewmodenumber");
	controlNumberField = ls->lookupField("controlnumber");
	fromNoteField = ls->lookupField("fromnote");
	toNoteField = ls->lookupField("tonote");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	controlNumberField.lock()->Hide(true);
	fromNoteField.lock()->Hide(true);
	toNoteField.lock()->Hide(true);
	controlNumberLabel = ls->lookupLabel("controlnumber");
	fromNoteLabel = ls->lookupLabel("fromnote");
	toNoteLabel = ls->lookupLabel("tonote");
	controlNumberLabel.lock()->Hide(true);

	if (lTrk->getBusNumber() != 0) {
		int pgm = sampler.lock()->getDrumBusProgramNumber(lTrk->getBusNumber());
		program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pgm).lock());
	}

	refreshViewModeNotes();
	setViewModeNotesText();

	lSequencer->addObserver(this);
	lTrk->addObserver(this);
	now0Field.lock()->setTextPadded(lSequencer->getCurrentBarNumber() + 1, "0");
	now1Field.lock()->setTextPadded(lSequencer->getCurrentBeatNumber() + 1, "0");
	now2Field.lock()->setTextPadded(lSequencer->getCurrentClockNumber(), "0");
	initVisibleEvents();
	eventRows.clear();
	for (int i = 0; i < 4; i++) {
		auto eventRow = make_unique<EventRow>(mpc, lTrk->getBusNumber(), visibleEvents[i], i);
		auto event = visibleEvents[i].lock();
		if (event) {
			event->addObserver(this);
		}
		if (lTrk->getBusNumber() == 0) eventRow->setMidi(true);
		eventRow->init();
		for (auto& ic : eventRow->getEventRow()) {
			ic.lock()->Hide(true);
		}
		eventRows.push_back(move(eventRow));
	}
	refreshEventRows();
	refreshSelection();
}

void StepEditorObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	auto ls = mpc->getLayeredScreen().lock();
	if (s.compare("viewmodestext") == 0) {
		setViewModeNotesText();
		viewField.lock()->setText(viewNames[stepEditorGui->getViewModeNumber()]);
		refreshViewModeNotes();
		setViewModeNotesText();
	}
	else if (s.compare("stepviewmodenumber") == 0) {
		viewField.lock()->setText(viewNames[stepEditorGui->getViewModeNumber()]);
		refreshViewModeNotes();
		setViewModeNotesText();
	}
	else if (s.compare("stepeditor") == 0) {
		if (mpc->getControls().lock()->getPressedPads()->size() != 0) {
			// a note is currently being recorded by the user pressing a pad
			initVisibleEvents();
			refreshEventRows();
			return;
		}
		auto focus = ls->getFocus();
		int eventNumber;
		try {
			eventNumber = stoi(focus.substr(1, 2));
		}
		catch (const std::invalid_argument& ia) {
			return;
		}
		if (dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber].lock())) {
			if (track.lock()->getBusNumber() != 0) {
				eventRows[eventNumber]->setDrumNoteEventValues();
			}
			else {
				eventRows[eventNumber]->setMidiNoteEventValues();
			}
		}
		else if (dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setMixerEventValues();
		}
		else if (dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber].lock())
			|| dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setMiscEventValues();
		}
		else if (dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setControlChangeEventValues();
		}
		else if (dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setChannelPressureEventValues();
		}
		else if (dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setPolyPressureEventValues();
		}
		else if (dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setSystemExclusiveEventValues();
		}
		else if (dynamic_pointer_cast<EmptyEvent>(visibleEvents[eventNumber].lock())) {
			eventRows[eventNumber]->setEmptyEventValues();
		}
	}
	else if (s.compare("resetstepeditor") == 0) {
		initVisibleEvents();
		refreshEventRows();
		refreshSelection();
	}
	else if (s.compare("bar") == 0) {
		if (sequencer.lock()->isPlaying()) {
			return;
		}
		now0Field.lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
		stepEditorGui->setyOffset(0);
	}
	else if (s.compare("beat") == 0) {
		if (sequencer.lock()->isPlaying()) {
			return;
		}
		now1Field.lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
		stepEditorGui->setyOffset(0);
	}
	else if (s.compare("clock") == 0) {
		if (sequencer.lock()->isPlaying()) {
			return;
		}
		now2Field.lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
		stepEditorGui->setyOffset(0);
	}
	else if (s.compare("selection") == 0) {
		refreshSelection();
	}
	else if (s.compare("selectionstart") == 0) {
		ls->drawFunctionKeys("sequencer_step_selection");
	}
	else if (s.compare("clearselection") == 0) {
		ls->drawFunctionKeys("sequencer_step");
	}
}

void StepEditorObserver::refreshSelection()
{
	auto firstEventIndex = stepEditorGui->getSelectionStartIndex();
	auto lastEventIndex = stepEditorGui->getSelectionEndIndex();
	bool somethingSelected = false;
	if (firstEventIndex != -1) {
		if (firstEventIndex > lastEventIndex) {
			firstEventIndex = stepEditorGui->getSelectionEndIndex();
			lastEventIndex = stepEditorGui->getSelectionStartIndex();
		}
		for (int i = 0; i < 4; i++) {
			int absoluteEventNumber = i + stepEditorGui->getyOffset();
			if (absoluteEventNumber >= firstEventIndex && absoluteEventNumber < lastEventIndex + 1) {
				eventRows[i]->setSelected(true);
				somethingSelected = true;
			}
			else {
				eventRows[i]->setSelected(false);
			}
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			eventRows[i]->setSelected(false);
		}
	}
	if (somethingSelected) mpc->getLayeredScreen().lock()->drawFunctionKeys("sequencer_step_selection");
}

void StepEditorObserver::initVisibleEvents()
{
	for (auto& e : eventsAtCurrentTick) {
		if (e.lock()) e.lock()->deleteObserver(this);
	}
	eventsAtCurrentTick.clear();
	auto lSequencer = sequencer.lock();
	auto lTrk = track.lock();
	for (auto& event : lTrk->getEvents()) {
		auto lEvent = event.lock();

		if (lEvent->getTick() == lSequencer->getTickPosition()) {
			if ((stepEditorGui->getViewModeNumber() == 0
			|| stepEditorGui->getViewModeNumber() == 1) 
			&& dynamic_pointer_cast<NoteEvent>(lEvent)) {
				auto ne = dynamic_pointer_cast<NoteEvent>(lEvent);
				if (lTrk->getBusNumber() != 0) {
					if (stepEditorGui->getFromNotePad() == 34) {
						eventsAtCurrentTick.push_back(ne);
					}
					else if (stepEditorGui->getFromNotePad() != 34
						&& stepEditorGui->getFromNotePad() == ne->getNote()) {
						eventsAtCurrentTick.push_back(ne);
					}
				}
				else {
					if (ne->getNote() >= stepEditorGui->getNoteA()
						&& ne->getNote() <= stepEditorGui->getNoteB()) {
						eventsAtCurrentTick.push_back(ne);
					}
				}
			}

			if ((stepEditorGui->getViewModeNumber() == 0 
			|| stepEditorGui->getViewModeNumber() == 2) 
			&& dynamic_pointer_cast<PitchBendEvent>(lEvent)) {
				eventsAtCurrentTick.push_back(event);
			}

			if ((stepEditorGui->getViewModeNumber() == 0 || stepEditorGui->getViewModeNumber() == 3)
				&& dynamic_pointer_cast<ControlChangeEvent>(lEvent)) {
				if (stepEditorGui->getControlNumber() == -1) {
					eventsAtCurrentTick.push_back(event);
				}
				if (stepEditorGui->getControlNumber() == dynamic_pointer_cast<ControlChangeEvent>(lEvent)->getController()) {
					eventsAtCurrentTick.push_back(event);
				}
			}
			if ((stepEditorGui->getViewModeNumber() == 0
				|| stepEditorGui->getViewModeNumber() == 4)
				&& dynamic_pointer_cast<ProgramChangeEvent>(lEvent)) {
				eventsAtCurrentTick.push_back(event);
			}

			if ((stepEditorGui->getViewModeNumber() == 0
				|| stepEditorGui->getViewModeNumber() == 5)
				&& dynamic_pointer_cast<ChannelPressureEvent>(lEvent)) {
				eventsAtCurrentTick.push_back(event);
			}

			if ((stepEditorGui->getViewModeNumber() == 0
				|| stepEditorGui->getViewModeNumber() == 6)
				&& dynamic_pointer_cast<PolyPressureEvent>(lEvent)) {
				eventsAtCurrentTick.push_back(event);
			}

			if ((stepEditorGui->getViewModeNumber() == 0
				|| stepEditorGui->getViewModeNumber() == 7)
				&& (dynamic_pointer_cast<SystemExclusiveEvent>(lEvent) 
				|| dynamic_pointer_cast<MixerEvent>(lEvent))) {
				eventsAtCurrentTick.push_back(event);
			}
			
		}
	}
	eventsAtCurrentTick.push_back(emptyEvent);
	stepEditorGui->setEventsAtCurrentTick(eventsAtCurrentTick);
	for (auto& e : visibleEvents) {
		if (e.lock()) e.lock()->deleteObserver(this);
	}
	visibleEvents = vector<weak_ptr<Event>>(4);
	int firstVisibleEventIndex = stepEditorGui->getyOffset();
	int visibleEventCounter = 0;
	for (int i = 0; i < 4; i++) {
		visibleEvents[visibleEventCounter] = eventsAtCurrentTick[i + firstVisibleEventIndex];
		visibleEventCounter++;
		if (visibleEventCounter > 3 || visibleEventCounter > eventsAtCurrentTick.size() - 1) break;
	}
	stepEditorGui->setVisibleEvents(visibleEvents);
}

void StepEditorObserver::refreshEventRows()
{
	for (int i = 0; i < 4; i++) {
		eventRows[i]->setEvent(visibleEvents[i]);
		auto event = visibleEvents[i].lock();
		eventRows[i]->init();
		if (!event) {
			for (auto& e : eventRows[i]->getEventRow()) {
				e.lock()->Hide(true);
			}
		}
		else {
			event->addObserver(this);
		}
	}
}

void StepEditorObserver::refreshViewModeNotes()
{
	auto lTrk = track.lock();
	if (stepEditorGui->getViewModeNumber() == 1 && lTrk->getBusNumber() != 0) {
		fromNoteLabel.lock()->Hide(false);
		fromNoteField.lock()->Hide(false);
		fromNoteField.lock()->setLocation(67, 0);
		toNoteLabel.lock()->Hide(true);
		toNoteField.lock()->Hide(true);
	}
	else if (stepEditorGui->getViewModeNumber() == 1 && lTrk->getBusNumber() == 0) {
		fromNoteLabel.lock()->Hide(false);
		fromNoteField.lock()->Hide(false);
		fromNoteField.lock()->setLocation(62, 0);
		fromNoteField.lock()->setSize(8 * 6, 9);
		toNoteField.lock()->setSize(8 * 6, 9);
		toNoteLabel.lock()->Hide(false);
		toNoteLabel.lock()->setText("-");
		toNoteField.lock()->Hide(false);
		controlNumberField.lock()->Hide(true);
	}
	else if (stepEditorGui->getViewModeNumber() == 3) {
		fromNoteLabel.lock()->Hide(true);
		fromNoteField.lock()->Hide(true);
		toNoteLabel.lock()->Hide(true);
		toNoteField.lock()->Hide(true);
		controlNumberField.lock()->Hide(false);
	}
	else if (stepEditorGui->getViewModeNumber() != 1 && stepEditorGui->getViewModeNumber() != 3) {
		fromNoteLabel.lock()->Hide(true);
		fromNoteField.lock()->Hide(true);
		toNoteLabel.lock()->Hide(true);
		toNoteField.lock()->Hide(true);
		controlNumberField.lock()->Hide(true);
	}
}

void StepEditorObserver::setViewModeNotesText()
{
	auto lTrk = track.lock();
	if (stepEditorGui->getViewModeNumber() == 1 && lTrk->getBusNumber() != 0) {
		if (stepEditorGui->getFromNotePad() != 34) {
			fromNoteField.lock()->setText(to_string(stepEditorGui->getFromNotePad()) + "/" + sampler.lock()->getPadName(program.lock()->getPadNumberFromNote(stepEditorGui->getFromNotePad())));
		}
		else {
			fromNoteField.lock()->setText("ALL");
		}
	}
	else if (stepEditorGui->getViewModeNumber() == 1 && lTrk->getBusNumber() == 0) {
		fromNoteField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(stepEditorGui->getNoteA()), " ", 3) + "(" + mpc::ui::Uis::noteNames[stepEditorGui->getNoteA()] + u8"\u00D4");
		toNoteField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(stepEditorGui->getNoteB()), " ", 3) + "(" + mpc::ui::Uis::noteNames[stepEditorGui->getNoteB()] + u8"\u00D4");
	}
	else if (stepEditorGui->getViewModeNumber() == 3) {
		if (stepEditorGui->getControlNumber() == -1) controlNumberField.lock()->setText("   -    ALL");
		if (stepEditorGui->getControlNumber() != -1) controlNumberField.lock()->setText(EventRow::controlNames[stepEditorGui->getControlNumber()]);
	}
	viewField.lock()->setText(viewNames[stepEditorGui->getViewModeNumber()]);
	viewField.lock()->setSize(viewField.lock()->getText().length() * 6 + 1, 9);
}

StepEditorObserver::~StepEditorObserver() {
	for (auto& e : visibleEvents) {
		if (e.lock()) e.lock()->deleteObserver(this);
	}
	stepEditorGui->deleteObserver(this);
	sequencer.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
}
