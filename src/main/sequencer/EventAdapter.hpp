#pragma once

#include <vector>
#include <memory>

namespace mpc { class Mpc; }

namespace ctoot::midi::core
{
	class ShortMessage;
	class MidiMessage;
}

namespace mpc::sequencer
{
	class Event;
	class NoteEvent;
	class MidiClockEvent;
	class Sequencer;
}

namespace mpc::sequencer
{
	class EventAdapter
	{

	private:
		mpc::Mpc& mpc;
		std::weak_ptr<Sequencer> sequencer;
		std::weak_ptr<Event> event;
		std::shared_ptr<MidiClockEvent> midiClockEvent;
		std::shared_ptr<NoteEvent> noteEvent;

	public:
		void process(ctoot::midi::core::MidiMessage* msg);

	private:
		std::weak_ptr<Event> convert(ctoot::midi::core::ShortMessage* msg);

	public:
		std::weak_ptr<Event> get();

		EventAdapter(mpc::Mpc& mpc, std::weak_ptr<Sequencer> sequencer);

	};
}
