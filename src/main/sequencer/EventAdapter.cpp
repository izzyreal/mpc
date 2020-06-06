#include "EventAdapter.hpp"

#include <Mpc.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>

#include <lcdgui/screens/window/MultiRecordingSetupLine.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/Screens.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace std;

EventAdapter::EventAdapter(weak_ptr<Sequencer> sequencer)
{
	this->sequencer = sequencer;
	midiClockEvent = make_shared<MidiClockEvent>(0);
	noteEvent = make_shared<NoteEvent>(35);
}

void EventAdapter::process(ctoot::midi::core::MidiMessage* msg)
{
	if (dynamic_cast<ctoot::midi::core::ShortMessage*>(msg) != nullptr) {
		event = convert(dynamic_cast<ctoot::midi::core::ShortMessage*>(msg));
	}

}

weak_ptr<Event> EventAdapter::convert(ctoot::midi::core::ShortMessage* msg)
{
	if (msg->getStatus() == ctoot::midi::core::ShortMessage::TIMING_CLOCK || msg->getStatus() == ctoot::midi::core::ShortMessage::START || msg->getStatus() == ctoot::midi::core::ShortMessage::STOP) {
		midiClockEvent->setStatus(msg->getStatus());
		return midiClockEvent;
	}

	auto midiInputScreen = dynamic_pointer_cast<MidiInputScreen>(Screens::getScreenComponent("midi-input"));

	if (midiInputScreen->getReceiveCh() != -1 && !(msg->getChannel() == midiInputScreen->getReceiveCh()))
	{
		return {};
	}

	if (msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_ON || msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_OFF)
	{

		noteEvent->setNote(msg->getData1());
	
		if (msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_ON)
		{
			noteEvent->setVelocity(msg->getData2());
		}
		else if (msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_OFF)
		{
			noteEvent->setVelocityZero();
		}

		auto track = sequencer.lock()->getActiveTrackIndex();
		
		auto screen = dynamic_pointer_cast<MultiRecordingSetupScreen>(Screens::getScreenComponent("multi-recording-setup"));
		auto mrs = screen->getMrsLines();

		if (sequencer.lock()->isRecordingModeMulti())
		{
			track = mrs[msg->getChannel()]->getTrack();
		}

		noteEvent->setTrack(track);
		noteEvent->setDuration(0);
		noteEvent->setVariationValue(64);
		return noteEvent;
	}
	return {};
}

weak_ptr<Event> EventAdapter::get()
{
    return event;
}
