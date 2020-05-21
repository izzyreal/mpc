#include <command/InsertEvent.hpp>

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

InsertEvent::InsertEvent(int insertEventType, mpc::sequencer::Track* track, mpc::sequencer::Sequencer* sequencer) 
{
	this->insertEventType = insertEventType;
	this->track = track;
	this->sequencer = sequencer;
}

void InsertEvent::execute()
{
	shared_ptr<mpc::sequencer::Event> event;
	if (insertEventType == 0)
	{
		event = track->addEvent(sequencer->getTickPosition(), "note").lock();
		auto noteEvent = dynamic_pointer_cast<NoteEvent>(event);
		noteEvent->setDuration(24);
		noteEvent->setNote(60);
		noteEvent->setVelocity(127);
		noteEvent->setVariationTypeNumber(0);
		noteEvent->setVariationValue(64);
	}
	else if (insertEventType == 1)
	{
		event = track->addEvent(sequencer->getTickPosition(), "pitchbend").lock();
		auto pitchBendEvent = dynamic_pointer_cast<PitchBendEvent>(event);
		pitchBendEvent->setAmount(0);
	}
	else if (insertEventType == 2)
	{
		event = track->addEvent(sequencer->getTickPosition(), "controlchange").lock();
		auto controlChangeEvent = dynamic_pointer_cast<ControlChangeEvent>(event);
		controlChangeEvent->setController(0);
		controlChangeEvent->setAmount(0);
	}
	else if (insertEventType == 3)
	{
		event = track->addEvent(sequencer->getTickPosition(), "programchange").lock();
		auto programChangeEvent = dynamic_pointer_cast<ProgramChangeEvent>(event);
		programChangeEvent->setProgram(1);
	}
	else if (insertEventType == 4)
	{
		event = track->addEvent(sequencer->getTickPosition(), "channelpressure").lock();
		auto channelPressureEvent = dynamic_pointer_cast<ChannelPressureEvent>(event);
		channelPressureEvent->setAmount(0);
	}
	else if (insertEventType == 5)
	{
		event = track->addEvent(sequencer->getTickPosition(), "polypressure").lock();
		auto polyPressureEvent = dynamic_pointer_cast<PolyPressureEvent>(event);
		polyPressureEvent->setNote(60);
		polyPressureEvent->setAmount(0);
	}
	else if (insertEventType == 6)
	{
		event = track->addEvent(sequencer->getTickPosition(), "systemexclusive").lock();
		auto systemExclusiveEvent = dynamic_pointer_cast<SystemExclusiveEvent>(event);
		systemExclusiveEvent->setByteB(247);
	}
	else if (insertEventType == 7)
	{
		event = track->addEvent(sequencer->getTickPosition(), "mixer").lock();
		auto mixerEvent = dynamic_pointer_cast<MixerEvent>(event);
		mixerEvent->setPadNumber(0);
		mixerEvent->setParameter(0);
		mixerEvent->setValue(0);
	}
}
