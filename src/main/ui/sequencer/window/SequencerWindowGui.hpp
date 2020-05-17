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
		int trackNumber = 0;
		int sq0 = 0;
		int sq1 = 0;
		int tr0 = 0;
		int tr1 = 0;
		std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges;
		int tempoChangeOffset = 0;
		int countIn = 0;
		bool inPlay = false;
		int rate = 0;
		bool waitForKey = false;
		bool inRec = false;
		int clickVolume = 0;
		int clickOutput = 0;
		int metronomeSound = 0;
		int accentVelo = 0;
		int accentNote = 0;
		int normalVelo = 0;
		int normalNote = 0;
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
		int getSq0();
		void setSq0(int i);
		int getSq1();
		void setSq1(int i);
		int getTr0();
		void setTr0(int i);
		int getTr1();
		void setTr1(int i);
		std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> getVisibleTempoChanges();
		void setVisibleTempoChanges(std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> vtc);
		int getTempoChangeOffset();
		void setTempoChangeOffset(int i);
		int getCountInMode();
		void setCountIn(int i);
		void setInPlay(bool b);
		bool getInPlay();
		int getRate();
		void setRate(int i);
		void setWaitForKey(bool b);
		bool isWaitForKeyEnabled();
		void setInRec(bool b);
		bool getInRec();
		int getClickVolume();
		void setClickVolume(int i);
		int getClickOutput();
		void setClickOutput(int i);
		int getMetronomeSound();
		void setMetronomeSound(int i);
		int getAccentNote();
		void setAccentNote(int i);
		int getAccentVelo();
		void setAccentVelo(int i);
		int getNormalNote();
		void setNormalNote(int i);
		int getNormalVelo();
		void setNormalVelo(int i);
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

		SequencerWindowGui();

	};
}
