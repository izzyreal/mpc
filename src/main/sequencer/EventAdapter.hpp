#pragma once

#include <vector>
#include <memory>

/**
 * Class for crude adaptation from MIDI event to MPC2000XL internal mpc::sequencer::Event.
 *
 * Any processing as per the MIDI Input screen, such as filtering by channel or event type,
 * is also performed by this class.
 *
 * If any of the filtering blocks the event from internal propagation, get() will return
 * an empty value. It is up to the consumer (at the time of writing only MpcMidiInput) to
 * check this.
 *
 * If the value is not empty, it will contain the emitted Event, which will have the
 * correct track as per the Multi Recording Setup screen, if Multi Track Recording
 * (M: rather than S: in the bottom left of the MAIN screen) is enabled.
 *
 * If Single Track Recording is enabled, the Event will have the active track associated
 * with it.
 */

namespace mpc { class Mpc; }

namespace mpc::engine::midi
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
		void process(mpc::engine::midi::MidiMessage* msg);

	private:
		std::weak_ptr<Event> convert(mpc::engine::midi::ShortMessage* msg);

	public:
		std::weak_ptr<Event> get();

		EventAdapter(mpc::Mpc& mpc, std::weak_ptr<Sequencer> sequencer);

	};
}
