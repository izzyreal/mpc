#include "InsertEventScreen.hpp"
#include "TimingCorrectScreen.hpp"

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

InsertEventScreen::InsertEventScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "insert-event", layerIndex)
{
}

void InsertEventScreen::open()
{
    eventAddedBeforeLeavingTheScreen = false;
	displayInsertEventType();
}

void InsertEventScreen::displayInsertEventType()
{
	findField("eventtype")->setText(eventTypeNames[insertEventType]);
}

void InsertEventScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 4:
		insertEvent();
        eventAddedBeforeLeavingTheScreen = true;
		openScreen("step-editor");
		break;
	}
}

void InsertEventScreen::insertEvent()
{
	init();

	if (insertEventType == 0)
	{
        const bool allowMultipleNotesOnSameTick = true;
		auto noteEvent = std::make_shared<NoteOnEvent>();
        track->addEvent(sequencer->getTickPosition(), noteEvent, allowMultipleNotesOnSameTick);
        auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
        unsigned short duration = timingCorrectScreen->getNoteValueLengthInTicks();
		noteEvent->setDuration(duration);
		noteEvent->setNote(60);
		noteEvent->setVelocity(127);
		noteEvent->setVariationType(NoteOnEvent::VARIATION_TYPE::TUNE_0);
		noteEvent->setVariationValue(64);
	}
	else if (insertEventType == 1)
	{
		auto pitchBendEvent = std::make_shared<PitchBendEvent>();
		track->addEvent(sequencer->getTickPosition(), pitchBendEvent);
		pitchBendEvent->setAmount(0);
	}
	else if (insertEventType == 2)
	{
		auto controlChangeEvent = std::make_shared<ControlChangeEvent>();
		track->addEvent(sequencer->getTickPosition(), controlChangeEvent);
		controlChangeEvent->setController(0);
		controlChangeEvent->setAmount(0);
	}
	else if (insertEventType == 3)
	{
		auto programChangeEvent = std::make_shared<ProgramChangeEvent>();
		track->addEvent(sequencer->getTickPosition(), programChangeEvent);
		programChangeEvent->setProgram(1);
	}
	else if (insertEventType == 4)
	{
		auto channelPressureEvent = std::make_shared<ChannelPressureEvent>();
		track->addEvent(sequencer->getTickPosition(), channelPressureEvent);
		channelPressureEvent->setAmount(0);
	}
	else if (insertEventType == 5)
	{
		auto polyPressureEvent = std::make_shared<PolyPressureEvent>();
		track->addEvent(sequencer->getTickPosition(), polyPressureEvent);
		polyPressureEvent->setNote(60);
		polyPressureEvent->setAmount(0);
	}
	else if (insertEventType == 6)
	{
		auto systemExclusiveEvent = std::make_shared<SystemExclusiveEvent>();
		track->addEvent(sequencer->getTickPosition(), systemExclusiveEvent);
		systemExclusiveEvent->setByteB(247);
	}
	else if (insertEventType == 7)
	{
		auto mixerEvent = std::make_shared<MixerEvent>();
		track->addEvent(sequencer->getTickPosition(), mixerEvent);
		mixerEvent->setPadNumber(0);
		mixerEvent->setParameter(0);
		mixerEvent->setValue(0);
	}
}

void InsertEventScreen::turnWheel(int i)
{
	init();
	
	if (param == "eventtype")
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

bool InsertEventScreen::isEventAddedBeforeLeavingTheScreen()
{
    return eventAddedBeforeLeavingTheScreen;
}
