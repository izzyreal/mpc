#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
	class PolyPressureEvent
		: public Event
	{

	private:
		int note{ 0 };
		int polyPressureValue{ 0 };

	public:
		void setNote(int i);
		int getNote() const;
		void setAmount(int i);
		int getAmount() const;

		PolyPressureEvent() = default;
		PolyPressureEvent(const PolyPressureEvent& event);
		void CopyValuesTo(std::weak_ptr<Event> dest) override;

		std::string getTypeName() override { return "poly-pressure"; }

	};
}
