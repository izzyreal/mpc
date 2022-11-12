#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
	class PitchBendEvent
		: public Event
	{

	private:
		int pitchBendAmount{ 0 };

	public:
		void setAmount(int i);
		int getAmount();

		void CopyValuesTo(std::weak_ptr<Event> dest) override;

		std::string getTypeName() override { return "pitch-bend"; }

	};
}
