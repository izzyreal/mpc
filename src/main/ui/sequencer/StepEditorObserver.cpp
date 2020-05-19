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

StepEditorObserver::StepEditorObserver()
{
	
	emptyEvent = make_shared<EmptyEvent>();
	viewNames = { "ALL EVENTS", "NOTES", "PITCH BEND", "CTRL:", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE" };
	stepEditorGui = Mpc::instance().getUis().lock()->getStepEditorGui();
	stepEditorGui->addObserver(this);
	sequencer = Mpc::instance().getSequencer();
	sampler = Mpc::instance().getSampler();
	auto lSequencer = sequencer.lock();
	auto seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	auto seq = sequence.lock();
	track = seq->getTrack(lSequencer->getActiveTrackIndex());
	auto lTrk = track.lock();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	viewField = ls->lookupField("viewmodenumber");
	controlNumberField = ls->lookupField("controlnumber");
	fromNoteField = ls->lookupField("fromnote");
	toNoteField = ls->lookupField("tonote");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");

}

void StepEditorObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	auto ls = Mpc::instance().getLayeredScreen().lock();
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
		if (Mpc::instance().getControls().lock()->getPressedPads()->size() != 0) {
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

StepEditorObserver::~StepEditorObserver() {
	for (auto& e : visibleEvents) {
		if (e.lock()) e.lock()->deleteObserver(this);
	}
	stepEditorGui->deleteObserver(this);
	sequencer.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
}
