#include <command/InsertEvent.hpp>

#include <ui/sequencer/StepEditorGui.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

using namespace mpc::command;
using namespace mpc::sequencer;
using namespace std;

InsertEvent::InsertEvent(mpc::ui::sequencer::StepEditorGui* seGui, mpc::sequencer::Track* track, mpc::sequencer::Sequencer* sequencer) 
{
	this->seGui = seGui;
	this->track = track;
	this->sequencer = sequencer;
}

void InsertEvent::execute()
{
	weak_ptr<mpc::sequencer::Event> event;
	if (seGui->getInsertEventType() == 0) {
		event = track->addEvent(sequencer->getTickPosition(), "note");
		auto lEvent = dynamic_pointer_cast<NoteEvent>(event.lock());
		lEvent->setDuration(24);
		lEvent->setNote(60);
		lEvent->setVelocity(127);
		lEvent->setVariationTypeNumber(0);
		lEvent->setVariationValue(64);
	}
	else if (seGui->getInsertEventType() == 1) {
		event = track->addEvent(sequencer->getTickPosition(), "pitchbend");
		auto lEvent = dynamic_pointer_cast<PitchBendEvent>(event.lock());
		lEvent->setAmount(0);
	}
	else if (seGui->getInsertEventType() == 2) {
		event = track->addEvent(sequencer->getTickPosition(), "controlchange");
		auto lEvent = dynamic_pointer_cast<ControlChangeEvent>(event.lock());
		lEvent->setController(0);
		lEvent->setAmount(0);
	}
	else if (seGui->getInsertEventType() == 3) {
		event = track->addEvent(sequencer->getTickPosition(), "programchange");
		auto lEvent = dynamic_pointer_cast<ProgramChangeEvent>(event.lock());
		lEvent->setProgram(1);
	}
	else if (seGui->getInsertEventType() == 4) {
		event = track->addEvent(sequencer->getTickPosition(), "channelpressure");
		auto lEvent = dynamic_pointer_cast<ChannelPressureEvent>(event.lock());
		lEvent->setAmount(0);
	}
	else if (seGui->getInsertEventType() == 5) {
		event = track->addEvent(sequencer->getTickPosition(), "polypressure");
		auto lEvent = dynamic_pointer_cast<PolyPressureEvent>(event.lock());
		lEvent->setNote(60);
		lEvent->setAmount(0);
	}
	else if (seGui->getInsertEventType() == 6) {
		event = track->addEvent(sequencer->getTickPosition(), "systemexclusive");
		auto lEvent = dynamic_pointer_cast<SystemExclusiveEvent>(event.lock());
		lEvent->setByteB(247);
	}
	else if (seGui->getInsertEventType() == 7) {
		event = track->addEvent(sequencer->getTickPosition(), "mixer");
		auto lEvent = dynamic_pointer_cast<MixerEvent>(event.lock());
		lEvent->setPadNumber(0);
		lEvent->setParameter(0);
		lEvent->setValue(0);
	}
}
