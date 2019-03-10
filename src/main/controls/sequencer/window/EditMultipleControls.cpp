#include "EditMultipleControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>

using namespace mpc::controls::sequencer::window;
using namespace mpc::sequencer;
using namespace std;

EditMultipleControls::EditMultipleControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void EditMultipleControls::function(int i)
{
	super::function(i);
	auto sEvent = seGui->getSelectedEvent();
	string paramLetter = seGui->getParamLetter();
	auto lTrk = track.lock();
	switch (i) {
	case 4:
		if (dynamic_pointer_cast<NoteEvent>(sEvent.lock()) && lTrk->getBusNumber() != 0) {
			if (paramLetter.compare("a") == 0) {
				checkNotes();
			}
			else if (paramLetter.compare("b") == 0) {
				for (auto& event : seGui->getSelectedEvents()) {
					if (dynamic_pointer_cast<NoteEvent>(event.lock()))
						dynamic_pointer_cast<NoteEvent>(event.lock())->setVariationTypeNumber(seGui->getChangeVariationTypeNumber());
				}
			}
			else if (paramLetter.compare("c") == 0) {
				for (auto& event : seGui->getSelectedEvents()) {
					if (dynamic_pointer_cast<NoteEvent>(event.lock()))
						dynamic_pointer_cast<NoteEvent>(event.lock())->setVariationValue(seGui->getChangeVariationValue());
				}
			}
			else if (paramLetter.compare("d") == 0) {
				checkFiveParameters();
			}
			else if (paramLetter.compare("e") == 0) {
				checkThreeParameters();
			}
		}

		if (dynamic_pointer_cast<NoteEvent>(sEvent.lock()) && lTrk->getBusNumber() == 0) {
			if (paramLetter.compare("a") == 0) {
				checkNotes();
			}
			else if (paramLetter.compare("b") == 0) {
				checkFiveParameters();
			}
			else if (paramLetter.compare("c") == 0) {
				checkThreeParameters();
			}
		}

		if (dynamic_pointer_cast<ControlChangeEvent>(sEvent.lock())) {
			if (paramLetter.compare("a") == 0) {
				checkFiveParameters();
			}
			else if (paramLetter.compare("b") == 0) {
				checkThreeParameters();
			}
		}
		if (dynamic_pointer_cast<ProgramChangeEvent>(sEvent.lock()) || dynamic_pointer_cast<ChannelPressureEvent>(sEvent.lock()))
			checkFiveParameters();

		if (dynamic_pointer_cast<PolyPressureEvent>(sEvent.lock())) {
			if (paramLetter.compare("a") == 0) {
				checkFiveParameters();
			}
			else if (paramLetter.compare("b") == 0) {
				checkThreeParameters();
			}

		}
		seGui->clearSelection();
		ls.lock()->openScreen("sequencer_step");
	}
}

void EditMultipleControls::turnWheel(int i)
{
	init();
	auto sEvent = seGui->getSelectedEvent();
	auto lTrk = track.lock();
	string paramLetter = seGui->getParamLetter();

	if (param.compare("value0") == 0) {
		if (dynamic_pointer_cast<NoteEvent>(sEvent.lock()) && lTrk->getBusNumber() != 0) {
			if (paramLetter.compare("a") == 0) {
				if (seGui->getChangeNoteToNumber() == 98) return;
				seGui->setChangeNoteToNumber(seGui->getChangeNoteToNumber() + i);
			}
			else if (paramLetter.compare("b") == 0) {
				seGui->setChangeVariationTypeNumber(seGui->getChangeVariationTypeNumber() + i);
			}
			else if (paramLetter.compare("c") == 0) {
				seGui->setChangeVariationValue(seGui->getChangeVariationValue() + i);
			}
			else if (paramLetter.compare("d") == 0 || paramLetter.compare("e") == 0) {
				seGui->setEditTypeNumber(seGui->getEditTypeNumber() + i);
			}
		}
		else if (dynamic_pointer_cast<NoteEvent>(sEvent.lock()) && lTrk->getBusNumber() == 0) {
			if (paramLetter.compare("a") == 0) {
				seGui->setChangeNoteToNumber(seGui->getChangeNoteToNumber() + i);
			}
			else if (paramLetter.compare("b") == 0 || paramLetter.compare("c") == 0) {
				seGui->setEditTypeNumber(seGui->getEditTypeNumber() + i);
			}
		}
		else if (dynamic_pointer_cast<ProgramChangeEvent>(sEvent.lock())
			|| dynamic_pointer_cast<PolyPressureEvent>(sEvent.lock())
			|| dynamic_pointer_cast<ChannelPressureEvent>(sEvent.lock())
			|| dynamic_pointer_cast<ControlChangeEvent>(sEvent.lock())) {
			seGui->setEditTypeNumber(seGui->getEditTypeNumber() + i);
		}
	}
	else if (param.compare("value1") == 0) {
		seGui->setEditValue(seGui->getEditValue() + 1);
	}
}

void EditMultipleControls::checkThreeParameters()
{
	for (auto& event : seGui->getSelectedEvents()) {

		auto note = dynamic_pointer_cast<NoteEvent>(event.lock());
		auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(event.lock());
		auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(event.lock());

		if (note) {
			note->setVelocity(seGui->getEditValue());
		}
		else if (controlChange) {
			controlChange->setAmount(seGui->getEditValue());
		}
		else if (polyPressure) {
			polyPressure->setAmount(seGui->getEditValue());
		}
	}
}

void EditMultipleControls::checkFiveParameters()
{
	for (auto& event : seGui->getSelectedEvents()) {

		auto note = dynamic_pointer_cast<NoteEvent>(event.lock());
		auto programChange = dynamic_pointer_cast<ProgramChangeEvent>(event.lock());
		auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(event.lock());
		auto channelPressure = dynamic_pointer_cast<ChannelPressureEvent>(event.lock());
		auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(event.lock());

		if (note) {
			note->setDuration(seGui->getEditValue());
		}
		else if (programChange) {
			programChange->setProgram(seGui->getEditValue());
		}
		else if (controlChange) {
			controlChange->setController(seGui->getEditValue());
		}
		else if (channelPressure) {
			channelPressure->setAmount(seGui->getEditValue());
		}
		else if (polyPressure) {
			polyPressure->setNote(seGui->getEditValue());
		}
	}
}

void EditMultipleControls::checkNotes()
{
	for (auto& event : seGui->getSelectedEvents()) {
		auto note = dynamic_pointer_cast<NoteEvent>(event.lock());
		if (note) {
			note->setNote(seGui->getChangeNoteToNumber());
		}
	}
}
