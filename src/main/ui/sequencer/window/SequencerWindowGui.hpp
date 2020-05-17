#pragma once
#include <observer/Observable.hpp>

#include <sequencer/TimeSignature.hpp>

#include <vector>

namespace mpc::sequencer {
	class TempoChangeEvent;
}

namespace mpc::ui::sequencer::window {

	class SequencerWindowGui
		: public moduru::observer::Observable
	{

	private:
		int tapAvg = 0;
		bool pgmChangeToSeqEnabled = false;

	public:
		int getTapAvg();
		void setTapAvg(int i);
		bool isPgmChangeToSeqEnabled();
		void setPgmChangeToSeqEnabled(bool b);

	};
}
