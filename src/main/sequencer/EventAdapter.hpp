#pragma once

#include <vector>
#include <memory>

namespace ctoot::midi::core
{
	class ShortMessage;
	class MidiMessage;
}

namespace mpc::ui::sequencer::window
{
	class SequencerWindowGui;
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
		std::weak_ptr<Sequencer> sequencer;
		std::weak_ptr<Event> event;
		mpc::ui::sequencer::window::SequencerWindowGui* swGui = nullptr;
		std::shared_ptr<MidiClockEvent> midiClockEvent;
		std::shared_ptr<NoteEvent> noteEvent;

	public:
		void process(ctoot::midi::core::MidiMessage* msg, mpc::ui::sequencer::window::SequencerWindowGui* gui);

	private:
		std::weak_ptr<Event> convert(ctoot::midi::core::ShortMessage* msg);

	public:
		std::weak_ptr<Event> get();

		EventAdapter(std::weak_ptr<Sequencer> sequencer);
		~EventAdapter();

	};
}
