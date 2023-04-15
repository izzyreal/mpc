//#include "EventAdapter.hpp"
//
//#include <Mpc.hpp>
//#include <sequencer/Event.hpp>
//#include <sequencer/MidiClockEvent.hpp>
//#include <sequencer/NoteEvent.hpp>
//#include <sequencer/Sequencer.hpp>
//#include <engine/midi/MidiMessage.hpp>
//#include <engine/midi/ShortMessage.hpp>
//
//#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
//#include <lcdgui/screens/window/MidiInputScreen.hpp>
//
//using namespace mpc::lcdgui;
//using namespace mpc::lcdgui::screens::window;
//using namespace mpc::sequencer;
//using namespace mpc::engine::midi;
//
//EventAdapter::EventAdapter(mpc::Mpc& mpc, std::weak_ptr<Sequencer> sequencer)
//	: mpc(mpc)
//{
//	this->sequencer = sequencer;
//	midiClockEvent = std::make_shared<MidiClockEvent>(0);
//	noteEvent = std::make_shared<OldNoteEvent>(35);
//}
//
//void EventAdapter::process(MidiMessage* msg)
//{
//	if (dynamic_cast<ShortMessage*>(msg) != nullptr)
//		event = convert(dynamic_cast<ShortMessage*>(msg));
//}
//
//std::weak_ptr<Event> EventAdapter::convert(ShortMessage* msg)
//{
//	if (msg->isMidiClock())
//	{
//		midiClockEvent->setStatus(msg->getStatus());
//		return midiClockEvent;
//	}
//
//	auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");
//
//	if (midiInputScreen->getReceiveCh() != -1 && msg->getChannel() != midiInputScreen->getReceiveCh())
//		return {};
//
//	if (msg->isNoteOn() || msg->isNoteOff())
//	{
//		//noteEvent->setNote(msg->getData1());
//	
//		//if (msg->getStatus() >= ShortMessage::NOTE_ON)
//		//	noteEvent->setVelocity(msg->getData2());
//		//else
//		//	noteEvent->setVelocityZero();
//
//		auto track = sequencer.lock()->getActiveTrackIndex();
//		
//		auto screen = mpc.screens->get<MultiRecordingSetupScreen>("multi-recording-setup");
//		auto mrs = screen->getMrsLines();
//
//		if (sequencer.lock()->isRecordingModeMulti())
//			track = mrs[msg->getChannel()]->getTrack();
//
//		noteEvent->setTrack(track);
//		noteEvent->setDuration(0);
//		return noteEvent;
//	}
//
//	return {};
//}
//
//std::weak_ptr<Event> EventAdapter::get()
//{
//    return event;
//}
