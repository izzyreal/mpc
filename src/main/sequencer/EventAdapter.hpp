#pragma once

#include <vector>
#include <memory>

namespace ctoot {
	namespace midi {
		namespace core {
			class ShortMessage;
			class MidiMessage;
		}
	}
}

namespace mpc {

	namespace ui {
		namespace sequencer {
			namespace window {
				class SequencerWindowGui;
				class MultiRecordingSetupLine;
			}
		}
	}

	namespace sequencer {

		class Event;
		class NoteEvent;
		class MidiClockEvent;
		class Sequencer;

		class EventAdapter
		{

		private:
			std::weak_ptr<Sequencer> sequencer{ };
			std::weak_ptr<Event> event {};
			mpc::ui::sequencer::window::SequencerWindowGui* swGui{ nullptr };
			std::vector<mpc::ui::sequencer::window::MultiRecordingSetupLine*> mrs{ nullptr };
			std::shared_ptr<MidiClockEvent> midiClockEvent{};
			std::shared_ptr<NoteEvent> noteEvent{};

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
}
