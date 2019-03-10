#pragma once
#include <observer/Observable.hpp>

#include <sequencer/TimeSignature.hpp>

#include <vector>

namespace mpc {
	namespace sequencer {
		class TempoChangeEvent;
	}

	namespace ui {

		namespace sequencer {
			namespace window {

				class MultiRecordingSetupLine;

				class SequencerWindowGui
					: public moduru::observer::Observable
				{

				private:
					int trackNumber{};
					int sq0{};
					int sq1{};
					int tr0{};
					int tr1{};
					int displayStyle{};
					int h{};
					int startTime{};
					int m{};
					int f{};
					int s{};
					int frameRate{};
					std::vector<MultiRecordingSetupLine*> mrsLines{};
					int mrsYOffset{};
					std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges{};
					int tempoChangeOffset{};
					int amount{};
					bool shiftTimingLater{};
					int drumNote{};
					int time0{};
					int time1{};
					int midiNote0{};
					int midiNote1{};
					int swing{};
					int noteValue{};
					int bar0{};
					int bar1{};
					mpc::sequencer::TimeSignature newTimeSignature;
					int countIn{};
					bool inPlay{};
					int rate{};
					bool waitForKey{};
					bool inRec{};
					int changeBarsLastBar{};
					int changeBarsFirstBar{};
					int changeBarsNumberOfBars{};
					int changeBarsAfterBar{};
					bool transmitProgramChangesInThisTrack{};
					int newBars{};
					std::vector<MultiRecordingSetupLine*> visibleMrsLines{};
					int receiveCh{};
					bool progChangeSeq{};
					bool sustainPedalToDuration{};
					bool midiFilter{};
					bool pass{};
					int filterType{};
					int softThru{};
					int deviceNumber{};
					int value{};
					int editType{};
					int clickVolume{};
					int clickOutput{};
					int metronomeSound{};
					int accentVelo{};
					int accentNote{};
					int normalVelo{};
					int normalNote{};
					bool notePassEnabled{};
					bool pitchBendPassEnabled{};
					bool pgmChangePassEnabled{};
					bool chPressurePassEnabled{};
					bool polyPressurePassEnabled{};
					bool exclusivePassEnabled{};
					int tapAvg{};
					bool pgmChangeToSeqEnabled{};

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
					int getDisplayStyle();
					void setDisplayStyle(int i);
					int getH();
					void setH(int i);
					int getStartTime();
					void setStartTime(int i);
					int getM();
					void setM(int i);
					int getF();
					void setF(int i);
					int getS();
					void setS(int i);
					int getFrameRate();
					void setFrameRate(int i);
					std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> getVisibleTempoChanges();
					void setVisibleTempoChanges(std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> vtc);
					int getTempoChangeOffset();
					void setTempoChangeOffset(int i);
					int getAmount();
					void setAmount(int i);
					bool isShiftTimingLater();
					void setShiftTimingLater(bool b);
					int getDrumNote();
					void setDrumNote(int i);
					int getMidiNote0();
					void setMidiNote0(int i);
					int getMidiNote1();
					void setMidiNote1(int i);
					int getSwing();
					void setSwing(int i);
					int getNoteValue();
					void setNoteValue(int i);
					int getTime0();
					void setTime0(int time0);
					int getTime1();
					void setTime1(int time1);
					int getBar0();
					void setBar0(int i, int max);
					int getBar1();
					void setBar1(int i, int max);
					mpc::sequencer::TimeSignature* getNewTimeSignature();
					void setNewTimeSignature(mpc::sequencer::TimeSignature ts);
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
					int getChangeBarsLastBar();
					void setChangeBarsLastBar(int i, int max);
					int getChangeBarsFirstBar();
					void setChangeBarsFirstBar(int i, int max);
					int getChangeBarsNumberOfBars();
					void setChangeBarsNumberOfBars(int i, int max);
					int getChangeBarsAfterBar();
					void setChangeBarsAfterBar(int i, int max);
					bool getTransmitProgramChangesInThisTrack();
					void setTransmitProgramChangesInThisTrack(bool b);
					int getNewBars();
					void setNewBars(int i);
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
		}
	}
}
