#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace sampler {

		class PgmSlider
			: public moduru::observer::Observable
		{

		private:
			int assignNote{ 34 };
			int tuneLowRange{ 0 };
			int tuneHighRange{ 0 };
			int decayLowRange{ 0 };
			int decayHighRange{ 0 };
			int attackLowRange{ 0 };
			int attackHighRange{ 0 };
			int filterLowRange{ 0 };
			int filterHighRange{ 0 };
			int controlChange{ 0 };
			int parameter{ 0 };

		public:
			void setAssignNote(int i);
			int getNote();
			void setTuneLowRange(int i);
			int getTuneLowRange();
			void setTuneHighRange(int i);
			int getTuneHighRange();
			void setDecayLowRange(int i);
			int getDecayLowRange();
			void setDecayHighRange(int i);
			int getDecayHighRange();
			void setAttackLowRange(int i);
			int getAttackLowRange();
			void setAttackHighRange(int i);
			int getAttackHighRange();
			void setFilterLowRange(int i);
			int getFilterLowRange();
			void setFilterHighRange(int i);
			int getFilterHighRange();
			void setControlChange(int i);
			int getControlChange();
			int getParameter();
			void setParameter(int i);

			PgmSlider();
			~PgmSlider();
		};

	}
}
