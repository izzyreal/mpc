//#include <sequencer/MidiAdapter.hpp>
//
//#include <sequencer/Event.hpp>
//#include <sequencer/NoteEvent.hpp>
//#include <engine/midi/ShortMessage.hpp>
//
//using namespace mpc::sequencer;
//
//MidiAdapter::MidiAdapter()
//{
//	message = std::make_shared<mpc::engine::midi::ShortMessage>();
//}
//
//void MidiAdapter::process(std::weak_ptr<Event> event, int channel, int newVelo)
//{
//	auto note = std::dynamic_pointer_cast<OldNoteEvent>(event.lock());
//	if (note) {
//		convert(note.get(), channel, newVelo);
//	}
//}
//
//void MidiAdapter::convert(OldNoteEvent* event, int channel, int newVelo)
//{
//	auto messageType = mpc::engine::midi::ShortMessage::NOTE_ON;
//	if (event->getVelocity() == 0) {
//		messageType = mpc::engine::midi::ShortMessage::NOTE_OFF;
//	}
//	message->setMessage(messageType, channel, event->getNote(), newVelo == -1 ? event->getVelocity() : newVelo);
//}
//
//std::weak_ptr<mpc::engine::midi::ShortMessage> MidiAdapter::get()
//{
//	return message;
//}
//
//MidiAdapter::~MidiAdapter() {
//}
