#pragma once
#include <sequencer/Event.hpp>

namespace mpc {
	namespace sequencer {

		class PolyPressureEvent
			: public Event
		{

		private:
			int note{ 0 };
			int polyPressureValue{ 0 };

		public:
			void setNote(int i);
			int getNote();
			void setAmount(int i);
			int getAmount();

			void CopyValuesTo(std::weak_ptr<Event> dest) override;

			PolyPressureEvent();

		};

	}
}
