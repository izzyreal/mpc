#include <sequencer/MidiAdapter.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>
#include "midi/ShortMessage.hpp"

using namespace mpc::sequencer;

MidiAdapter::MidiAdapter() 
{
	message = std::make_shared<ctoot::midi::core::ShortMessage>();
}

void MidiAdapter::process(std::weak_ptr<Event> event, int channel, int newVelo)
{
	auto note = std::dynamic_pointer_cast<NoteEvent>(event.lock());
	if (note) {
		convert(note.get(), channel, newVelo);
	}
}

void MidiAdapter::convert(NoteEvent* event, int channel, int newVelo)
{
	auto messageType = ctoot::midi::core::ShortMessage::NOTE_ON;
	if (event->getVelocity() == 0) {
		messageType = ctoot::midi::core::ShortMessage::NOTE_OFF;
	}
	message->setMessage(messageType, channel, event->getNote(), newVelo == -1 ? event->getVelocity() : newVelo);
}

std::weak_ptr<ctoot::midi::core::ShortMessage> MidiAdapter::get()
{
	return message;
}

MidiAdapter::~MidiAdapter() {
}
