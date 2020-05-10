#include "StepEditorControls.hpp"

#include <controls/Controls.hpp>

#include <Mpc.hpp>
#include <audiomidi/EventHandler.hpp>
#include <command/CopySelectedNote.hpp>
#include <command/CopySelectedNotes.hpp>
#include <command/RemoveEvents.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/EmptyEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

using namespace mpc::controls::sequencer;
using namespace mpc::sequencer;
using namespace std;

StepEditorControls::StepEditorControls()
	: AbstractSequencerControls()
{
	seGui = Mpc::instance().getUis().lock()->getStepEditorGui();
}

void StepEditorControls::init()
{
	super::init();
	visibleEvents = seGui->getVisibleEvents();
}

void StepEditorControls::function(int i)
{
	init();
	auto lTrk = track.lock();
	switch (i) {
	case 0:
		ls.lock()->openScreen("step_tc");
		break;
	case 1:
		if (seGui->getSelectionStartIndex() != -1) {
			auto command = mpc::command::CopySelectedNotes(seGui);
			command.execute();
		}
		else if (seGui->getSelectionStartIndex() == -1 && param.length() == 2) {
			auto command = mpc::command::CopySelectedNote(param, seGui);
			command.execute();
		}
		break;
	case 2:
		if (seGui->getSelectionStartIndex() != -1) {
			auto command = mpc::command::RemoveEvents(seGui, track.lock().get());
			command.execute();
			ls.lock()->openScreen("sequencer_step");
			ls.lock()->setFocus("a0");
		}
		else if (param.length() == 2) {
			auto eventNumber = stoi(param.substr(1, 2));
			if (!dynamic_pointer_cast<EmptyEvent>(visibleEvents[eventNumber].lock())) {
				for (int i = 0; i < lTrk->getEvents().size(); i++) {
					if (lTrk->getEvents()[i].lock() == visibleEvents[eventNumber].lock()) {
						lTrk->removeEvent(i);
						break;
					}
				}
				ls.lock()->openScreen("sequencer_step");
			}
		}
		break;
	case 3:
	{
		bool posIsLastTick = sequencer.lock()->getTickPosition() == sequencer.lock()->getActiveSequence().lock()->getLastTick();
		if (seGui->getSelectionEndIndex() == -1) {
			if (!posIsLastTick) {
				ls.lock()->openScreen("insertevent");
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

			if (pbe || me || sysex) return;

			bool isA = eventLetter.compare("a") == 0;
			bool isB = eventLetter.compare("b") == 0;
			bool isC = eventLetter.compare("c") == 0;
			bool isD = eventLetter.compare("d") == 0;
			bool isE = eventLetter.compare("e") == 0;

			if ((ppe || cce) && isA) return;

			if (ne && lTrk->getBusNumber() != 0) {
				if (isA) {
					seGui->setChangeNoteToNumber(ne->getNote());
				}
				else if (isB) {
					seGui->setChangeVariationTypeNumber(ne->getVariationTypeNumber());
				}
				else if (isC) {
					seGui->setChangeVariationTypeNumber(ne->getVariationTypeNumber());
					seGui->setChangeVariationValue(ne->getVariationValue());
				}
				else if (isD) {
					seGui->setEditValue(ne->getDuration());
				}
				else if (isE) {
					seGui->setEditValue(ne->getVelocity());
				}
			}

			if (ne && lTrk->getBusNumber() == 0) {
				if (isA) {
					seGui->setChangeNoteToNumber(ne->getNote());
				}
				else if (isB) {
					seGui->setEditValue(ne->getDuration());
				}
				else if (isC) {
					seGui->setEditValue(ne->getVelocity());
				}
			}
			else if (pce) {
				seGui->setEditValue(pce->getProgram());
			}
			else if (cpe) {
				seGui->setEditValue(cpe->getAmount());
			}
			else if (ppe) {
				seGui->setEditValue(ppe->getAmount());
			}
			else if (cce) {
				seGui->setEditValue(cce->getAmount());
			}
			seGui->setSelectedEvent(visibleEvents[eventNumber]);
			seGui->setSelectedEvents();
			seGui->setSelectedParameterLetter(eventLetter);
			ls.lock()->openScreen("editmultiple");
			break;
		}
	}
	case 4:
		if (seGui->getPlaceHolder().size() != 0)
			ls.lock()->openScreen("pasteevent");
		break;
	case 5:
		if (seGui->getSelectionStartIndex() == -1) {
			if (param.length() == 2) {
				auto eventNumber = stoi(param.substr(1, 2));
				auto event = seGui->getVisibleEvents()[eventNumber].lock();
				auto empty = dynamic_pointer_cast<EmptyEvent>(event);
				if (!empty) {
					auto tick = event->getTick();
					event->setTick(-1);
					Mpc::instance().getEventHandler().lock()->handle(event, lTrk.get());
					event->setTick(tick);
				}
			}
		}
		else {
			seGui->clearSelection();
		}
		break;
	}
}

void StepEditorControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	auto lTrk = track.lock();
	if (param.compare("viewmodenumber") == 0) {
		seGui->setViewModeNumber(seGui->getViewModeNumber() + i);
	}
	else if (param.compare("now0") == 0) {
		lSequencer->setBar(lSequencer->getCurrentBarNumber() + i);
	}
	else if (param.compare("now1") == 0) {
		lSequencer->setBeat(lSequencer->getCurrentBeatNumber() + i);
	}
	else if (param.compare("now2") == 0) {
		lSequencer->setClock(lSequencer->getCurrentClockNumber() + i);
	}
	else if (param.compare("tcvalue") == 0) {
		swGui->setNoteValue(swGui->getNoteValue() + i);
	}
	else if (param.compare("fromnote") == 0) {
		if (lTrk->getBusNumber() != 0) seGui->setFromNotePad(seGui->getFromNotePad() + i);
		if (lTrk->getBusNumber() == 0) seGui->setNoteA(seGui->getNoteA() + i);
	}
	else if (param.compare("tonote") == 0) {
		seGui->setNoteB(seGui->getNoteB() + i);
	}
	else if (param.compare("controlnumber") == 0) {
		seGui->setControlNumber(seGui->getControlNumber() + i);
	}
	else if (param.length() == 2) {

		auto eventNumber = stoi(param.substr(1, 2));

		auto sysEx = dynamic_pointer_cast<SystemExclusiveEvent>(visibleEvents[eventNumber].lock());
		auto channelPressure = dynamic_pointer_cast<ChannelPressureEvent>(visibleEvents[eventNumber].lock());
		auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(visibleEvents[eventNumber].lock());
		auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(visibleEvents[eventNumber].lock());
		auto programChange = dynamic_pointer_cast<ProgramChangeEvent>(visibleEvents[eventNumber].lock());
		auto pitchBend = dynamic_pointer_cast<PitchBendEvent>(visibleEvents[eventNumber].lock());
		auto mixer = dynamic_pointer_cast<MixerEvent>(visibleEvents[eventNumber].lock());
		auto note = dynamic_pointer_cast<NoteEvent>(visibleEvents[eventNumber].lock());

		if (sysEx) {
			if (param.find("a") != string::npos) {
				sysEx->setByteA(sysEx->getByteA() + i);
			}
			else if (param.find("b") != string::npos) {
				sysEx->setByteB(sysEx->getByteB() + i);
			}
		}
		else if (channelPressure) {
			if (param.find("a") != string::npos) {
				channelPressure->setAmount(channelPressure->getAmount() + i);
			}
		}
		else if (polyPressure) {
			if (param.find("a") != string::npos) {
				polyPressure->setNote(polyPressure->getNote() + i);
			}
			else if (param.find("b") != string::npos) {
				polyPressure->setAmount(polyPressure->getAmount() + i);
			}
		}
		else if (controlChange) {
			if (param.find("a") != string::npos) {
				controlChange->setController(controlChange->getController() + i);
			}
			else if (param.find("b") != string::npos) {
				controlChange->setAmount(controlChange->getAmount() + i);
			}

		}
		else if (programChange) {
			if (param.find("a") != string::npos) {
				programChange->setProgram(programChange->getProgram() + i);
			}

		}
		else if (pitchBend) {
			if (param.find("a") != string::npos) {
				pitchBend->setAmount(pitchBend->getAmount() + i);
			}

		}
		else if (mixer) {
			if (param.find("a") != string::npos) {
				mixer->setParameter(mixer->getParameter() + i);
			}
			else if (param.find("b") != string::npos) {
				mixer->setPadNumber(mixer->getPad() + i);
			}
			else if (param.find("c") != string::npos) {
				mixer->setValue(mixer->getValue() + i);
			}
		}
		else if (note && lTrk->getBusNumber() == 0) {
			if (param.find("a") != string::npos) {
				note->setNote(note->getNote() + i);
			}
			else if (param.find("b") != string::npos) {
				note->setDuration(note->getDuration() + i);
			}
			else if (param.find("c") != string::npos) {
				note->setVelocity(note->getVelocity() + i);
			}
		}
		else if (note && lTrk->getBusNumber() != 0) {
			if (param.find("a") != string::npos) {
				if (note->getNote() + i > 98) {
					if (note->getNote() != 98) note->setNote(98);
					return;
				}
				if (note->getNote() + i < 35) {
					if (note->getNote() != 35) note->setNote(35);
					return;
				}

				if (note->getNote() < 35) {
					note->setNote(35);
					return;
				}
				if (note->getNote() > 98) {
					note->setNote(98);
					return;
				}
				
				note->setNote(note->getNote() + i);
			}
			else if (param.find("b") != string::npos) {
				note->setVariationTypeNumber(note->getVariationTypeNumber() + i);
			}
			else if (param.find("c") != string::npos) {
				note->setVariationValue(note->getVariationValue() + i);
			}
			else if (param.find("d") != string::npos) {
				note->setDuration(note->getDuration() + i);
			}
			else if (param.find("e") != string::npos) {
				note->setVelocity(note->getVelocity() + i);
			}
		}
	}
	seGui->setChanged();
	seGui->notifyObservers(string("selection"));
}

void StepEditorControls::prevStepEvent()
{
	init();
	auto lSequencer = sequencer.lock();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isGoToPressed()) {
		lSequencer->goToPreviousEvent();
	}
	else {
		lSequencer->goToPreviousStep();
	}
}

void StepEditorControls::nextStepEvent()
{
	init();
	auto lSequencer = sequencer.lock();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isGoToPressed()) {
		lSequencer->goToNextEvent();
	}
	else {
		lSequencer->goToNextStep();
	}
}

void StepEditorControls::prevBarStart()
{
	init();
	auto controls = Mpc::instance().getControls().lock();
	auto lSequencer = sequencer.lock();
	if (controls->isGoToPressed()) {
		lSequencer->setBar(0);
	}
	else {
		lSequencer->setBar(lSequencer->getCurrentBarNumber() - 1);
	}
}

void StepEditorControls::nextBarEnd()
{
	init();
	auto lSequencer = sequencer.lock();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isGoToPressed()) {
		lSequencer->setBar(lSequencer->getActiveSequence().lock()->getLastBar() + 1);
	}
	else {
		lSequencer->setBar(lSequencer->getCurrentBarNumber() + 1);
	}
}

void StepEditorControls::left() {
	super::left();
	seGui->checkSelection();
	seGui->setChanged();
	seGui->notifyObservers(string("selection"));
}

void StepEditorControls::right() {
	super::right();
	seGui->checkSelection();
	seGui->setChanged();
	seGui->notifyObservers(string("selection"));
}

void StepEditorControls::up() {
	init();
	auto lLs = ls.lock();
	if (param.length() == 2) {
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto increment = 0;
		auto controls = Mpc::instance().getControls().lock();
		if (!controls->isShiftPressed() && srcNumber == 0 && seGui->getyOffset() == 0) {
			seGui->clearSelection();
			lLs->setFocus("viewmodenumber");
			seGui->setChanged();
			seGui->notifyObservers(string("selection"));
			return;
		}
		if (srcNumber == 0 && seGui->getyOffset() != 0) {
			seGui->setyOffset(seGui->getyOffset() - 1);
			if (controls->isShiftPressed()) seGui->setSelectionEndIndex(srcNumber + seGui->getyOffset());
		}
		increment = -1;
		downOrUp(increment);
	}
}

void StepEditorControls::down() {
	init();
	auto lLs = ls.lock();
	if (!lLs->lookupField("a0").lock()->IsHidden() && param.compare("viewmodenumber") == 0) {
		lLs->setFocus("a0");
		return;
	}
	else if (param.find("now") != string::npos) {
		if (!lLs->lookupLabel("e0").lock()->IsHidden()) {
			lLs->setFocus("e0");
		}
		else if (!lLs->lookupLabel("d0").lock()->IsHidden()) {
			lLs->setFocus("d0");
		}
		else if (!lLs->lookupLabel("c0").lock()->IsHidden()) {
			lLs->setFocus("c0");
		}
		else if (!lLs->lookupLabel("b0").lock()->IsHidden()) {
			lLs->setFocus("b0");
		}
		else if (!lLs->lookupLabel("a0").lock()->IsHidden()) {
			lLs->setFocus("a0");
		}
		return;
	}

	if (param.length() == 2) {
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto increment = 0;
		auto controls = Mpc::instance().getControls().lock();
		if (srcNumber == 3) {
			if (seGui->getyOffset() + 4 == seGui->getEventsAtCurrentTick().size()) return;
			seGui->setyOffset(seGui->getyOffset() + 1);
			if (controls->isShiftPressed() && dynamic_pointer_cast<EmptyEvent>(visibleEvents[3].lock())) {
				seGui->setSelectionEndIndex(srcNumber + seGui->getyOffset());
			}
			seGui->setChanged();
			seGui->notifyObservers(string("selection"));
			return;
		}
		increment = 1;
		downOrUp(increment);
	}
}

void StepEditorControls::shift() {
	init();
	super::shift();
	if (param.length() == 2) {
		auto eventNumber = stoi(param.substr(1, 2));
		seGui->setSelectionStartIndex(eventNumber + seGui->getyOffset());
	}
}

void StepEditorControls::downOrUp(int increment) {
	if (param.length() == 2) {
		auto lLs = ls.lock();
		auto src = param;
		auto srcLetter = src.substr(0, 1);
		int srcNumber = stoi(src.substr(1, 2));
		auto controls = Mpc::instance().getControls().lock();
		auto destination = srcLetter + to_string(srcNumber + increment);
		if (srcNumber + increment != -1) {
			if (!(controls->isShiftPressed() && dynamic_pointer_cast<EmptyEvent>(visibleEvents[(int)(srcNumber + increment)].lock()))) {
				auto tf = lLs->lookupField(destination).lock();
				if (tf && !tf->IsHidden()) lLs->setFocus(tf->getName());
			}
		}

		if (controls->isShiftPressed()) {
			seGui->setSelectionEndIndex(srcNumber + increment + seGui->getyOffset());
		}
		else {
			seGui->checkSelection();
			seGui->setChanged();
			seGui->notifyObservers(string("selection"));
		}
	}
}
