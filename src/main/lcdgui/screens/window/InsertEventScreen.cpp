#include "InsertEventScreen.hpp"

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace std;

InsertEventScreen::InsertEventScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "insert-event", layerIndex)
{
}

void InsertEventScreen::open()
{
	displayInsertEventType();
}

void InsertEventScreen::displayInsertEventType()
{
	findField("eventtype").lock()->setText(eventTypeNames[insertEventType]);
}

void InsertEventScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 4:
	{
		insertEvent();
		openScreen("step-editor");
		break;
	}
	}
}

void InsertEventScreen::insertEvent()
{
	init();
	if (insertEventType == 0)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "note");
		auto noteEvent = dynamic_pointer_cast<NoteEvent>(event);
		noteEvent->setDuration(24);
		noteEvent->setNote(60);
		noteEvent->setVelocity(127);
		noteEvent->setVariationTypeNumber(0);
		noteEvent->setVariationValue(64);
	}
	else if (insertEventType == 1)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "pitchbend");
		auto pitchBendEvent = dynamic_pointer_cast<PitchBendEvent>(event);
		pitchBendEvent->setAmount(0);
	}
	else if (insertEventType == 2)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "controlchange");
		auto controlChangeEvent = dynamic_pointer_cast<ControlChangeEvent>(event);
		controlChangeEvent->setController(0);
		controlChangeEvent->setAmount(0);
	}
	else if (insertEventType == 3)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "programchange");
		auto programChangeEvent = dynamic_pointer_cast<ProgramChangeEvent>(event);
		programChangeEvent->setProgram(1);
	}
	else if (insertEventType == 4)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "channelpressure");
		auto channelPressureEvent = dynamic_pointer_cast<ChannelPressureEvent>(event);
		channelPressureEvent->setAmount(0);
	}
	else if (insertEventType == 5)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "polypressure");
		auto polyPressureEvent = dynamic_pointer_cast<PolyPressureEvent>(event);
		polyPressureEvent->setNote(60);
		polyPressureEvent->setAmount(0);
	}
	else if (insertEventType == 6)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "systemexclusive");
		auto systemExclusiveEvent = dynamic_pointer_cast<SystemExclusiveEvent>(event);
		systemExclusiveEvent->setByteB(247);
	}
	else if (insertEventType == 7)
	{
		auto event = track.lock()->addEvent(sequencer->getTickPosition(), "mixer");
		auto mixerEvent = dynamic_pointer_cast<MixerEvent>(event);
		mixerEvent->setPadNumber(0);
		mixerEvent->setParameter(0);
		mixerEvent->setValue(0);
	}
}

void InsertEventScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("eventtype") == 0)
	{
		setInsertEventType(insertEventType + i);
	}
}

void InsertEventScreen::setInsertEventType(int i)
{
	if (i < 0 || i > 7)
	{
		return;
	}
	insertEventType = i;
	displayInsertEventType();
}
