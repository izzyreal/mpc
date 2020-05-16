#pragma once
#include <observer/Observable.hpp>

#include <sequencer/TimeSignature.hpp>

#include <vector>

namespace mpc::sequencer {
	class TempoChangeEvent;
}

namespace mpc::ui::sequencer::window {
	class MultiRecordingSetupLine;
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
		std::vector<MultiRecordingSetupLine*> mrsLines;
		int mrsYOffset = 0;
		std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges;
		int tempoChangeOffset = 0;
		int time0 = 0;
		int time1 = 0;
		int midiNote0 = 0;
		int midiNote1 = 127;
		int countIn = 0;
		bool inPlay = false;
		int rate = 0;
		bool waitForKey = false;
		bool inRec = false;
		bool transmitProgramChangesInThisTrack = false;
		std::vector<MultiRecordingSetupLine*> visibleMrsLines;
		int receiveCh = 0;
		bool progChangeSeq = false;
		bool sustainPedalToDuration = false;
		bool midiFilter = false;
		bool pass = false;
		int filterType = 0;
		int softThru = 0;
		int deviceNumber = 0;
		int value = 0;
		int editType = 0;
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
		int getMidiNote0();
		void setMidiNote0(int i);
		int getMidiNote1();
		void setMidiNote1(int i);
		int getTime0();
		void setTime0(int time0);
		int getTime1();
		void setTime1(int time1);
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
		bool getTransmitProgramChangesInThisTrack();
		void setTransmitProgramChangesInThisTrack(bool b);
		std::vector<MultiRecordingSetupLine*> getMrsLines();
		void setMrsYOffset(int i);
		int getMrsYOffset();
		void setMrsTrack(int inputNumber, int newTrackNumber);
		void setMrsOut(int inputNumber, int newOutputNumber);
		void setVisbleMrsLines(std::vector<MultiRecordingSetupLine*>* newVisibleMrsLines);
		std::vector<MultiRecordingSetupLine*>* getVisibleMrsLines();
		void setReceiveCh(int i);
		int getReceiveCh();
		void setProgChangeSeq(bool b);
		bool getProgChangeSeq();
		void setSustainPedalToDuration(bool b);
		bool isSustainPedalToDurationEnabled();
		void setMidiFilterEnabled(bool b);
		bool isMidiFilterEnabled();
		void setFilterType(int i);
		int getMidiFilterType();
		void setPass(bool b);
		bool getPass();
		void setSoftThru(int i);
		int getSoftThru();
		void setDeviceNumber(int i);
		int getDeviceNumber();
		int getEditType();
		void setEditType(int i);
		int getValue();
		void setValue(int i);
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
		~SequencerWindowGui();

	};
}
