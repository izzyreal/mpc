#include <sequencer/EventAdapter.hpp>

#include <Mpc.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>

#include <lcdgui/screens/window/MultiRecordingSetupLine.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
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

void EventAdapter::process(ctoot::midi::core::MidiMessage* msg, mpc::ui::sequencer::window::SequencerWindowGui* swGui)
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
	if (swGui->getReceiveCh() != -1 && !(msg->getChannel() == swGui->getReceiveCh())) {
		return weak_ptr<Event>();
	}

	if (msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_ON || msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_OFF) {

		noteEvent->setNote(msg->getData1());
		if (msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_ON) {
			noteEvent->setVelocity(msg->getData2());
		}
		else if (msg->getStatus() == ctoot::midi::core::ShortMessage::NOTE_OFF) {
			noteEvent->setVelocityZero();
		}

		auto lSequencer = sequencer.lock();
		auto track = lSequencer->getActiveTrackIndex();
		
		auto screen = dynamic_pointer_cast<MultiRecordingSetupScreen>(Screens::getScreenComponent("multirecordingsetup"));
		auto mrs = screen->getMrsLines();

		if (lSequencer->isRecordingModeMulti())
		{
			track = mrs[msg->getChannel()]->getTrack();
		}

		noteEvent->setTrack(track);
		noteEvent->setDuration(0);
		noteEvent->setVariationValue(64);
		return noteEvent;
	}
	return weak_ptr<Event>();
}

weak_ptr<Event> EventAdapter::get()
{
    return event;
}

EventAdapter::~EventAdapter() {
}
