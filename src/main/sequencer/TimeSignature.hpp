#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace sequencer {

		class TimeSignature
			: public moduru::observer::Observable
		{

		private:
			int numerator;
			int denominator;

		public:
			void setNumerator(int i);
			int getNumerator();
			void setDenominator(int i);
			int getDenominator();
			void increase();
			void decrease();

			TimeSignature();

		};

	}
}
