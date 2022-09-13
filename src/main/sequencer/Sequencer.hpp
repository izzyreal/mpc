#pragma once
#include <sequencer/Sequence.hpp>

#include <observer/Observable.hpp>

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>

namespace mpc { class Mpc; }

namespace mpc::sequencer
{
	class Song;
}

namespace mpc::sequencer
{
	class Sequencer final
		: public moduru::observer::Observable
	{

	public:
		Sequencer(mpc::Mpc& mpc);
        
        std::vector<int> getTickValues();
        void playToTick(int targetTick);
        int getActiveSequenceIndex();
        std::weak_ptr<Track> getActiveTrack();
        std::weak_ptr<Sequence> createSeqInPlaceHolder();
        void clearPlaceHolder();
        void movePlaceHolderTo(int destIndex);
        std::weak_ptr<Sequence> getPlaceHolder();
        bool isUndoSeqAvailable();

	private:
        const std::vector<int> TICK_VALUES{ 1, 48, 32, 24, 16, 12, 8 };
        mpc::Mpc& mpc;
		int lastNotifiedBar = -1;
		int lastNotifiedBeat = -1;
		int lastNotifiedClock = -1;
		std::shared_ptr<Sequence> placeHolder;
		bool metronomeOnly = false;
		std::unique_ptr<mpc::sequencer::Sequence> metronomeSeq;
		int activeSequenceIndex = 0;
		int currentlyPlayingSequenceIndex = 0;
		bool songMode = false;
		int playedStepRepetitions = 0;
		bool endOfSong = false;

		std::vector<std::shared_ptr<Sequence>> sequences;
		std::vector<std::shared_ptr<Song>> songs;
		std::vector<uint64_t> taps{ 0, 0, 0, 0 };

		std::shared_ptr<Sequence> undoPlaceHolder;

		bool secondSequenceEnabled = false;
		bool undoSeqAvailable = false;
		int playStartTick = 0;
		double previousTempo = 0.0;
		int recordStartTick = 0;

		std::string defaultSequenceName = "";
		std::vector<std::string> defaultDeviceNames;
		int timeDisplayStyle = 0;
		bool recordingModeMulti = false;
		int frameRate = 0;
		bool countEnabled = false;
		bool soloEnabled = false;
		bool tempoSourceSequenceEnabled = false;

		bool countingIn = false;
		int reposition = 0;
		int position = 0;
		uint64_t lastTap = 0;
		int tapIndex = 0;

		std::vector<std::string> defaultTrackNames;
		bool overdubbing = false;
		bool recording = false;
		int activeTrackIndex = 0;
		double tempo = 120.0;
		int nextSq = -1;

		std::weak_ptr<TempoChangeEvent> getCurrentTempoChangeEvent();
		void play(bool fromStart);
		std::shared_ptr<Sequence> copySequence(std::weak_ptr<Sequence> source);
		void copySequenceParameters(std::weak_ptr<Sequence> source, std::weak_ptr<Sequence> dest);
		void copyTempoChangeEvents(std::weak_ptr<Sequence> src, std::weak_ptr<Sequence> dst);
		void copyTrackParameters(std::weak_ptr<Track> source, std::weak_ptr<Track> dest);
		void copyTrack(std::weak_ptr<Track> src, std::weak_ptr<Track> dest);

	public:
		void notifyTimeDisplay();
		void notifyTimeDisplayRealtime();
		void init();
		void setTempo(double newTempo);
		double getTempo();
		bool isTempoSourceSequenceEnabled();
		void setTempoSourceSequence(bool b);
		bool isRecordingOrOverdubbing();
		bool isRecording();
		bool isSoloEnabled();
		void setSoloEnabled(bool b);
		std::weak_ptr<Sequence> getSequence(int i);
		std::string getDefaultSequenceName();
		void setDefaultSequenceName(std::string s);
		void setActiveSequenceIndex(int i);
		std::string getDefaultDeviceName(int i);
		void setDefaultDeviceName(int i, std::string s);
		bool isCountEnabled();
		void setCountEnabled(bool b);
		void setTimeDisplayStyle(int i);
		int getTimeDisplayStyle();
		void setRecordingModeMulti(bool b);
		bool isRecordingModeMulti();
		int getActiveTrackIndex();
		void trackUp();
		void trackDown();
		bool isPlaying();
		int getPlayedStepRepetitions();
		bool isEndOfSong();
		void setEndOfSong(bool b);
		void incrementPlayedStepRepetitions();
		void resetPlayedStepRepetitions();

	public:
		void undoSeq();
		void clearUndoSeq();
		void playFromStart();
		void play();
		void rec();
		void recFromStart();
		void overdub();
		void switchRecordToOverDub();
		void overdubFromStart();
		void stop();
		void stop(int tick);
		bool isCountingIn();
		void setCountingIn(bool b);
		void notifyTrack();
		void setSequence(int i, std::shared_ptr<Sequence> s);
		void purgeAllSequences();
		void purgeSequence(int i);
		void copySequence(int sq0, int sq1);
		void copySequenceParameters(const int source, const int dest);
		void copySong(const int source, const int dest);

	public:
		void copyTrack(int sourceTrackIndex, int destinationTrackIndex, int sourceSequenceIndex, int destinationSequenceIndex);
		std::vector<std::string> getDefaultTrackNames();
		std::string getDefaultTrackName(int i);
		void setDefaultTrackName(std::string s, int i);
		int getCurrentBarIndex();
		int getCurrentBeatIndex();
		int getCurrentClockNumber();
		void setBar(int i);
		void setBeat(int i);
		void setClock(int i);
		int getLoopEnd();
		std::weak_ptr<Sequence> getActiveSequence();
		int getUsedSequenceCount();
		std::vector<std::weak_ptr<Sequence>> getUsedSequences();
		std::vector<int> getUsedSequenceIndexes();
		void goToPreviousEvent();
		void goToNextEvent();
		void goToPreviousStep();
		void goToNextStep();
		void tap();
		int getResolution();
		void move(int tick);
		int getTickPosition();
		std::weak_ptr<Sequence> getCurrentlyPlayingSequence();
		void setActiveTrackIndex(int i);
		int getCurrentlyPlayingSequenceIndex();
		void setCurrentlyPlayingSequenceIndex(int i);
		int getNextSq();
		int getFirstUsedSeqDown(int from, bool unused = false);
		int getFirstUsedSeqUp(int from, bool unused = false);
		void setNextSq(int i);
		void setNextSqPad(int i);
		std::weak_ptr<Song> getSong(int i);
		void deleteSong(int i);
		bool isSongModeEnabled();
		void setSongModeEnabled(bool b);
		int getSongSequenceIndex();
		bool isSecondSequenceEnabled();
		void setSecondSequenceEnabled(bool b);
		void flushTrackNoteCache();
		void storeActiveSequenceInUndoPlaceHolder();
		bool isOverDubbing();
		int getPlayStartTick();

		void notify(std::string s);

		void setRecording(bool b);
		void setOverdubbing(bool b);
		void playMetronomeTrack();
		void stopMetronomeTrack();

	};
}
