#pragma once
#include <observer/Observable.hpp>
#include <Mpc.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

#include <memory>

namespace mpc {

	namespace sequencer {

		class Track;
		class TimeSignature;
		class TempoChangeEvent;
		class Sequence
			: public moduru::observer::Observable
		{

		public:
			typedef moduru::observer::Observable super;

		private:
			Mpc* mpc{ nullptr };

			BCMath initialTempo = BCMath("120.0");

			std::vector<std::shared_ptr<Track>> tracks{};
			std::vector<std::shared_ptr<Track>> metaTracks{};

			std::vector<std::string> deviceNames{};
			std::vector<std::string> defaultTrackNames{};

			std::vector<int> barLengths{ };
			std::vector<int> numerators{ };
			std::vector<int> denominators{ };

			std::string name{""};
			bool loopEnabled{false};
			int lastBar{ -1 };
			bool used{ false };
			bool tempoChangeOn{ false };
			int loopStart{ 0 };
			int loopEnd{ 0 };
			int firstLoopBar{ 0 };
			int lastLoopBar{ 0 };
			bool lastLoopBarEnd{ false };

			//file::all::AllParser* allFile{};

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

			static bool trackIndexComparator(std::weak_ptr<Track> t0, std::weak_ptr<Track> t1);

		public:
			bool isLoopEnabled();
			void setName(std::string s);
			std::string getName();
			void setDeviceName(int i, std::string s);
			std::string getDeviceName(int i);
			void setLastBar(int i);
			int getLastBar();
			void setLoopEnabled(bool b);
			void copyBars(int fromSequence, int firstBar, int lastBar, int toSequence, int afterBar, int copies);
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
			void deleteBars(int firstBar, int lBar);
			void insertBars(int numberOfBars, int afterBar);
			void moveTrack(int source, int destination);
			bool isLastLoopBarEnd();
			//void setTempoChangeEvents(std::vector<TempoChangeEvent*>* tceList);

			int getEventCount();
			void initLoop();
			std::vector<int>* getNumerators();
			std::vector<int>* getDenominators();
			void removeFirstMetronomeClick();
			int getNoteEventCount();

			int getFirstTickOfBar(int index);
			int getLastTickOfBar(int index);

		public:
			Sequence(Mpc* mpc, std::vector<std::string> defaultTrackNames);
			~Sequence();

		};

	}
}
