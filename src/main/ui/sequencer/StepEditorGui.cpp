#include <ui/sequencer/StepEditorGui.hpp>

#include <Mpc.hpp>
#include <lcdgui/SelectedEventBar.hpp>
#include <sequencer/EmptyEvent.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

StepEditorGui::StepEditorGui(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	viewModeNumber = 0;
	noteA = 0;
	noteB = 127;
	controlNumber = -1;
	autoStepIncrementEnabled = false;
	durationOfRecordedNotes = false;
	tcValueRecordedNotes = 100;
	yOffset = 0;
	selectedEventNumber = 0;
	insertEventType = 0;
	editTypeNumber = 0;
	changeNoteToNumber = 35;
	changeVariationTypeNumber = 0;
	changeVariationValue = 0;
	editValue = 0;
	fromNotePad = 34;
	selectionStartIndex = -1;
	selectionEndIndex = -1;
}

int StepEditorGui::getInsertEventType()
{
    return insertEventType;
}

void StepEditorGui::setInsertEventType(int i)
{
	if (i < 0 || i > 7) return;
	insertEventType = i;
	setChanged();
	notifyObservers(string("eventtype"));
}

int StepEditorGui::getViewModeNumber()
{
    return viewModeNumber;
}

void StepEditorGui::setViewModeNumber(int i)
{
	if (i < 0 || i > 7) return;

	this->viewModeNumber = i;
	setChanged();
	notifyObservers(string("stepviewmodenumber"));
	setyOffset(0);
}

int StepEditorGui::getNoteA()
{
    return noteA;
}

void StepEditorGui::setNoteA(int i)
{
	if (i < 0 || i > 127) return;

	this->noteA = i;
	if (i > noteB) setNoteB(i);
	setChanged();
	notifyObservers(string("viewmodestext"));
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

int StepEditorGui::getNoteB()
{
    return noteB;
}

void StepEditorGui::setNoteB(int i)
{
	if (i < 0 || i > 127) return;
	this->noteB = i;
	if (i < noteA) setNoteA(i);

	setChanged();
	notifyObservers(string("viewmodestext"));
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

int StepEditorGui::getControlNumber()
{
    return controlNumber;
}

void StepEditorGui::setControlNumber(int i)
{
	if (i < -1 || i > 127) return;

	this->controlNumber = i;
	setChanged();
	notifyObservers(string("viewmodestext"));
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

bool StepEditorGui::isAutoStepIncrementEnabled()
{
    return autoStepIncrementEnabled;
}

void StepEditorGui::setAutoStepIncrementEnabled(bool b)
{
    this->autoStepIncrementEnabled = b;
    setChanged();
    notifyObservers(string("autostepincrement"));
}

bool StepEditorGui::getDurationOfRecordedNotes()
{
    return durationOfRecordedNotes;
}

void StepEditorGui::setDurationOfRecordedNotes(bool b)
{
    this->durationOfRecordedNotes = b;
    setChanged();
    notifyObservers(string("durationofrecordednotes"));
}

int StepEditorGui::getTcValueRecordedNotes()
{
    return tcValueRecordedNotes;
}

void StepEditorGui::setTcValueRecordedNotes(int i)
{
    if(i < 0 || i > 100) return;

    this->tcValueRecordedNotes = i;
    setChanged();
    notifyObservers(string("tcvaluerecordednotes"));
}

int StepEditorGui::getyOffset()
{
    return yOffset;
}

void StepEditorGui::setyOffset(int i)
{
	if (i < 0) return;
	this->yOffset = i;
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

int StepEditorGui::getSelectedEventNumber()
{
    return selectedEventNumber;
}

void StepEditorGui::setSelectedEventNumber(int i)
{
    this->selectedEventNumber = i;
}

void StepEditorGui::setVisibleEvents(vector<weak_ptr<mpc::sequencer::Event>> e)
{
    visibleEvents = e;
}

vector<weak_ptr<mpc::sequencer::Event>> StepEditorGui::getVisibleEvents()
{
    return visibleEvents;
}

void StepEditorGui::setEventsAtCurrentTick(vector<weak_ptr<mpc::sequencer::Event>> l)
{
    eventsAtCurrentTick = l;
}

vector<weak_ptr<mpc::sequencer::Event>> StepEditorGui::getEventsAtCurrentTick()
{
    return eventsAtCurrentTick;
}

void StepEditorGui::setFromNotePad(int i)
{
	if (i < 34 || i > 98) return;
	fromNotePad = i;
	setChanged();
	notifyObservers(string("viewmodestext"));
	setChanged();
	notifyObservers(string("resetstepeditor"));
}

int StepEditorGui::getFromNotePad()
{
    return fromNotePad;
}

void StepEditorGui::setSelectionStartIndex(int i)
{
    if (dynamic_pointer_cast<mpc::sequencer::EmptyEvent>(eventsAtCurrentTick[i].lock())) return;
    selectionStartIndex = i;
    selectionEndIndex = i;
    setChanged();
    notifyObservers(string("selectionstart"));
    setChanged();
    notifyObservers(string("selection"));
}

void StepEditorGui::clearSelection()
{
    selectionStartIndex = -1;
    selectionEndIndex = -1;
    setChanged();
    notifyObservers(string("clearselection"));
    setChanged();
    notifyObservers(string("selection"));
}

int StepEditorGui::getSelectionStartIndex()
{
    return selectionStartIndex;
}

void StepEditorGui::setSelectionEndIndex(int i)
{
    if (i == -1) return;
    if (dynamic_pointer_cast<mpc::sequencer::EmptyEvent>(eventsAtCurrentTick[i].lock())) return;
    selectionEndIndex = i;
    setChanged();
    notifyObservers(string("selection"));
}

int StepEditorGui::getSelectionEndIndex()
{
    return selectionEndIndex;
}

void StepEditorGui::setSelectedEvents()
{
	selectedEvents.clear();
	auto firstEventIndex = selectionStartIndex;
	auto lastEventIndex = selectionEndIndex;
	if (firstEventIndex > lastEventIndex) {
		firstEventIndex = selectionEndIndex;
		lastEventIndex = selectionStartIndex;
	}
	for (int i = firstEventIndex; i < lastEventIndex + 1; i++)
		selectedEvents.push_back(eventsAtCurrentTick[i]);

}

vector<weak_ptr<mpc::sequencer::Event>> StepEditorGui::getSelectedEvents()
{
    return selectedEvents;
}

void StepEditorGui::setPlaceHolder(vector<weak_ptr<mpc::sequencer::Event>> l)
{
    placeHolder = l;
}

vector<weak_ptr<mpc::sequencer::Event>> StepEditorGui::getPlaceHolder()
{
    return placeHolder;
}

void StepEditorGui::checkSelection()
{
	auto ls = mpc->getLayeredScreen().lock();
	string focus = ls->getFocus();
	if (focus.length() == 2) {
		int eventNumber = stoi(focus.substr(1, 2));
		int  visibleEventCounter = 0;
		int firstSelectedVisibleEventIndex = -1;
		auto selectedEventCounter = 0;
		for (auto& seb : ls->getSelectedEventBarsStepEditor()) {
			if (!seb.lock()->IsHidden()) {
				if (firstSelectedVisibleEventIndex == -1) {
					firstSelectedVisibleEventIndex = visibleEventCounter;
				}
				selectedEventCounter++;
			}
			visibleEventCounter++;
		}
		if (firstSelectedVisibleEventIndex != -1) {
			int lastSelectedVisibleEventIndex = firstSelectedVisibleEventIndex + selectedEventCounter - 1;
			if (!dynamic_pointer_cast<mpc::sequencer::EmptyEvent>(visibleEvents[eventNumber].lock())) {
				if (eventNumber < firstSelectedVisibleEventIndex || eventNumber > lastSelectedVisibleEventIndex) {
					clearSelection();
				}
			}
		}
	}
	else {
		clearSelection();
	}
}

int StepEditorGui::getEditTypeNumber()
{
    return editTypeNumber;
}

void StepEditorGui::setEditTypeNumber(int i)
{
	if (i < 0 || i > 3) return;
    editTypeNumber = i;
    setChanged();
    notifyObservers(string("editmultiple"));
}

void StepEditorGui::setSelectedEvent(weak_ptr<mpc::sequencer::Event> event)
{
    selectedEvent = event;
}

weak_ptr<mpc::sequencer::Event> StepEditorGui::getSelectedEvent()
{
    return selectedEvent;
}

void StepEditorGui::setSelectedParameterLetter(string str)
{
    selectedParameterLetter = str;
}

string StepEditorGui::getParamLetter()
{
    return selectedParameterLetter;
}

int StepEditorGui::getChangeNoteToNumber()
{
    return changeNoteToNumber;
}

void StepEditorGui::setChangeNoteToNumber(int i)
{
	if (i < 0 || i > 127) return;
	this->changeNoteToNumber = i;
	setChanged();
	notifyObservers(string("editmultiple"));
}

int StepEditorGui::getChangeVariationTypeNumber()
{
    return changeVariationTypeNumber;
}

void StepEditorGui::setChangeVariationTypeNumber(int i)
{
	if (i < 0 || i > 3) return;
	this->changeVariationTypeNumber = i;
	setChanged();
	notifyObservers(string("editmultiple"));
}

int StepEditorGui::getChangeVariationValue()
{
    return changeVariationValue;
}

void StepEditorGui::setChangeVariationValue(int i)
{
	if (i < 0 || i > 128) return;
	if (changeVariationTypeNumber != 0 && i > 100) i = 100;
	this->changeVariationValue = i;
	setChanged();
	notifyObservers(string("editmultiple"));
}

int StepEditorGui::getEditValue()
{
    return editValue;
}

void StepEditorGui::setEditValue(int i)
{
	if (i < 0 || i > 127) return;

	this->editValue = i;
	setChanged();
	notifyObservers(string("editmultiple"));
}

bool StepEditorGui::isDurationTcPercentageEnabled()
{
    return durationTcPercentageEnabled;
}

StepEditorGui::~StepEditorGui() {
}
