#pragma once
#include <observer/Observable.hpp>
#include <Mpc.hpp>

#include <memory>

namespace mpc::sequencer {
	class Track;
	class TimeSignature;
	class TempoChangeEvent;
}

namespace mpc::sequencer {

	class Sequence final
		: public moduru::observer::Observable
	{

	private:
		double initialTempo = 120.0;

		std::vector<std::shared_ptr<Track>> tracks;
		std::vector<std::shared_ptr<Track>> metaTracks;

		std::vector<std::string> deviceNames;
		std::vector<std::string> defaultTrackNames;

		std::vector<int> barLengths;
		std::vector<int> numerators;
		std::vector<int> denominators;

		std::string name = "";
		bool loopEnabled = false;
		int lastBarIndex = -1;
		bool used = false;
		bool tempoChangeOn = false;
		int loopStart = 0;
		int loopEnd = 0;
		int firstLoopBarIndex = 0;
		int lastLoopBarIndex = 0;
		bool lastLoopBarEnd = false;

	public:
		double getInitialTempo();
		void setInitialTempo(const double initialTempo);

		void setLoopStart(int l);
		int getLoopStart();
		void setLoopEnd(int l);
		int getLoopEnd();
		void setFirstLoopBarIndex(int i);
		int getFirstLoopBarIndex();
		void setLastLoopBarIndex(int i);
		int getLastLoopBarIndex();
		void initMetaTracks();

	private:
		void createClickTrack();
		void createMidiClockTrack();
		void createTempoChangeTrack();

		static bool trackIndexComparator(std::weak_ptr<Track> t0, std::weak_ptr<Track> t1);

	public:
		bool isLoopEnabled();
		void setName(std::string s);
		std::string getName();
		void setDeviceName(int i, std::string s);
		std::string getDeviceName(int i);
		void setLastBar(int i);
		int getLastBarIndex();
		void setLoopEnabled(bool b);
		std::weak_ptr<Track> getTrack(int i);
		void setUsed(bool b);
		bool isUsed();
		void init(int lastBarIndex);
		void setTimeSignature(int firstBar, int tsLastBar, int num, int den);
		void setTimeSignature(int bar, int num, int den);
		std::vector<std::weak_ptr<Track>> getTracks();
		std::vector<std::weak_ptr<Track>> getMetaTracks();
		std::vector<std::string> getDeviceNames();
		void setDeviceNames(std::vector<std::string> sa);
		std::vector<std::weak_ptr<TempoChangeEvent>> getTempoChangeEvents();
		std::weak_ptr<TempoChangeEvent> addTempoChangeEvent();
		void removeTempoChangeEvent(int i);
		void removeTempoChangeEvent(std::weak_ptr<TempoChangeEvent> tce);

		bool isTempoChangeOn();
		void setTempoChangeOn(bool b);
		int getLastTick();
		TimeSignature getTimeSignature();
		void sortTempoChangeEvents();
		void sortTracks();
		void purgeAllTracks();
		std::weak_ptr<Track> purgeTrack(int i);
		int getDenominator(int i);
		int getNumerator(int i);
		std::vector<int>* getBarLengths();
		void setBarLengths(std::vector<int>&);
		void setNumeratorsAndDenominators(std::vector<int>& numerators, std::vector<int>& denominators);
		void deleteBars(int firstBar, int lBar);
		void insertBars(int numberOfBars, int afterBar);
		void moveTrack(int source, int destination);
		bool isLastLoopBarEnd();

		int getEventCount();
		void initLoop();
		std::vector<int>* getNumerators();
		std::vector<int>* getDenominators();
		void removeFirstMetronomeClick();
		int getNoteEventCount();

		int getFirstTickOfBar(int index);
		int getLastTickOfBar(int index);

		void resetTrackEventIndices(int tick);

	public:
		Sequence(std::vector<std::string> defaultTrackNames);

	};
}
