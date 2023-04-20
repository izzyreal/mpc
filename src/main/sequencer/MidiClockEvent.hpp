#pragma once
#include <sequencer/Event.hpp>
#include <engine/midi/ShortMessage.hpp>

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
		virtual int getStatus() const;
		virtual void setStatus(int i);

		MidiClockEvent(int statusToUse);
		MidiClockEvent(std::shared_ptr<mpc::engine::midi::ShortMessage> msg);
		MidiClockEvent(const MidiClockEvent&);

		void CopyValuesTo(std::weak_ptr<Event> dest) override;
		
		std::string getTypeName() override { return "midi-clock"; }

	};
}
