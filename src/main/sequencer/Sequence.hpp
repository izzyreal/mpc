#pragma once
#include <observer/Observable.hpp>
#include <Mpc.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

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
		BCMath initialTempo = BCMath("120.0");

		std::vector<std::shared_ptr<Track>> tracks;
		std::vector<std::shared_ptr<Track>> metaTracks;

		std::vector<string> deviceNames;
		std::vector<string> defaultTrackNames;

		std::vector<int> barLengths;
		std::vector<int> numerators;
		std::vector<int> denominators;

		string name = "";
		bool loopEnabled = false;
		int lastBar = -1;
		bool used = false;
		bool tempoChangeOn = false;
		int loopStart = 0;
		int loopEnd = 0;
		int firstLoopBar = 0;
		int lastLoopBar = 0;
		bool lastLoopBarEnd = false;

	public:
		BCMath getInitialTempo();
		void setInitialTempo(BCMath bd);

		void setLoopStart(int l);
		int getLoopStart();
		void setLoopEnd(int l);
		int getLoopEnd();
		void setFirstLoopBar(int i);
		int getFirstLoopBar();
		void setLastLoopBar(int i);
		int getLastLoopBar();
		void initMetaTracks();

	private:
		void createClickTrack();
		void createMidiClockTrack();
		void createTempoChangeTrack();

		static bool trackIndexComparator(weak_ptr<Track> t0, weak_ptr<Track> t1);

	public:
		bool isLoopEnabled();
		void setName(string s);
		string getName();
		void setDeviceName(int i, string s);
		string getDeviceName(int i);
		void setLastBar(int i);
		int getLastBar();
		void setLoopEnabled(bool b);
		weak_ptr<Track> getTrack(int i);
		void setUsed(bool b);
		bool isUsed();
		void init(int lastBarIndex);
		void setTimeSignature(int firstBar, int tsLastBar, int num, int den);
		void setTimeSignature(int bar, int num, int den);
		std::vector<weak_ptr<Track>> getTracks();
		std::vector<weak_ptr<Track>> getMetaTracks();
		std::vector<string> getDeviceNames();
		void setDeviceNames(std::vector<string> sa);
		std::vector<weak_ptr<TempoChangeEvent>> getTempoChangeEvents();
		weak_ptr<TempoChangeEvent> addTempoChangeEvent();
		void removeTempoChangeEvent(int i);
		void removeTempoChangeEvent(weak_ptr<TempoChangeEvent> tce);

		bool isTempoChangeOn();
		void setTempoChangeOn(bool b);
		int getLastTick();
		TimeSignature getTimeSignature();
		void sortTempoChangeEvents();
		void sortTracks();
		void purgeAllTracks();
		weak_ptr<Track> purgeTrack(int i);
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
		Sequence(std::vector<string> defaultTrackNames);

	};
}
