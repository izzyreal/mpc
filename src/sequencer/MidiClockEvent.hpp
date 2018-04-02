#pragma once
#include <sequencer/Event.hpp>
#include <midi/core/ShortMessage.hpp>

namespace mpc {
	namespace sequencer {

		class MidiClockEvent
			: public Event
		{

		public:
			typedef Event super;

		private:
			int status{ 0 };

		public:
			virtual int getStatus();
			virtual void setStatus(int i);

			void CopyValuesTo(std::weak_ptr<Event> dest) override;

			MidiClockEvent(int status);

		};

	}
}
