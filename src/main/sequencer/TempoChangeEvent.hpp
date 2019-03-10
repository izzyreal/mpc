#pragma once
#include <sequencer/Event.hpp>
#include <thirdp/bcmath/bcmath_stl.h>

namespace mpc {
	namespace sequencer {

		class Sequence;
		class TempoChangeEvent
			: public Event
		{

		private:
			int ratio{ 1000 };
			int stepNumber{ 0 };
			Sequence* parent{ nullptr };

		public:
			void plusOneBar(int numerator, int denominator, TempoChangeEvent* next);
			void minusOneBar(int numerator, int denominator, TempoChangeEvent* previous);
			void plusOneBeat(int denominator, TempoChangeEvent* next);
			void minusOneBeat(int denominator, TempoChangeEvent* previous);
			void plusOneClock(TempoChangeEvent* next);
			void minusOneClock(TempoChangeEvent* previous);
			void setRatio(int i);
			int getRatio();
			void setStepNumber(int i);
			int getStepNumber();
			int getBar(int n, int d);
			int getBeat(int n, int d);
			int getClock(int n, int d);
			BCMath getTempo();
//			BCMath getInitialTempo();
//			void setInitialTempo(BCMath bd);

			void CopyValuesTo(std::weak_ptr<Event> dest);

			TempoChangeEvent(Sequence* parent);
			TempoChangeEvent(Sequence* parent, int ratio);
			TempoChangeEvent(Sequence* parent, int ratio, int step);
		};

	}
}
