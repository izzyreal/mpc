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
		std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges;
		int tempoChangeOffset = 0;
		bool notePassEnabled = false;
		bool pitchBendPassEnabled = false;
		bool pgmChangePassEnabled = false;
		bool chPressurePassEnabled = false;
		bool polyPressurePassEnabled = false;
		bool exclusivePassEnabled = false;
		int tapAvg = 0;
		bool pgmChangeToSeqEnabled = false;

	public:
		int getTrackNumber();
		void setTrackNumber(int i);
		std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> getVisibleTempoChanges();
		void setVisibleTempoChanges(std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> vtc);
		int getTempoChangeOffset();
		void setTempoChangeOffset(int i);
		bool isNotePassEnabled();
		void setNotePassEnabled(bool b);
		bool isPitchBendPassEnabled();
		void setPitchBendPassEnabled(bool b);
		bool isPgmChangePassEnabled();
		void setPgmChangePassEnabled(bool b);
		bool isChPressurePassEnabled();
		void setChPressurePassEnabled(bool b);
		bool isPolyPressurePassEnabled();
		void setPolyPressurePassEnabled(bool b);
		bool isExclusivePassEnabled();
		void setExclusivePassEnabled(bool b);
		int getTapAvg();
		void setTapAvg(int i);
		bool isPgmChangeToSeqEnabled();
		void setPgmChangeToSeqEnabled(bool b);

	};
}
