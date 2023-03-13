#pragma once
#include <sequencer/Event.hpp>
#include "midi/ShortMessage.hpp"

namespace mpc::sequencer
{
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

		MidiClockEvent(int statusToUse);
		std::string getTypeName() override { return "midi-clock"; }

	};
}
