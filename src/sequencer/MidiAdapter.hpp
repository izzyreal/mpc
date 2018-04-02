#pragma once

#include <memory>

namespace ctoot {
	namespace midi {
		namespace core {
			class MidiMessage;
			class ShortMessage;
		}
	}
}

namespace mpc {
	namespace sequencer {

		class Event;
		class NoteEvent;

		class MidiAdapter
		{

		private:
			std::shared_ptr<ctoot::midi::core::ShortMessage> message{};

		public:
			void process(std::weak_ptr<Event> event, int channel, int newVelo);

		private:
			void convert(NoteEvent* event, int channel, int newVelo);

		public:
			std::weak_ptr<ctoot::midi::core::ShortMessage> get();

			MidiAdapter();
			~MidiAdapter();

		};

	}
}
