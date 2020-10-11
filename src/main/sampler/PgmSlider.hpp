#pragma once
#include <observer/Observable.hpp>

namespace mpc::sampler
{
	class PgmSlider
		: public moduru::observer::Observable
	{

	private:
		int assignNote = 34;
		int tuneLowRange = -120;
		int tuneHighRange = 120;
		int decayLowRange = 12;
		int decayHighRange = 45;
		int attackLowRange = 0;
		int attackHighRange = 20;
		int filterLowRange = -50;
		int filterHighRange = 50;
		int controlChange = 0;
		int parameter = 0;

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
	};
}
