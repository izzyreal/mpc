#pragma once

#include <ostream>
#include <istream>
#include <memory>
#include <vector>
#include <fstream>

namespace mpc::midi::event {
	class MidiEvent;
}

using namespace std;

namespace mpc::midi {
	class MidiTrack
	{

	private:
		static const bool VERBOSE{ false };
		static vector<char> IDENTIFIER;

	private:
		int mSize{};
		bool mSizeNeedsRecalculating{};
		bool mClosed{};
		int mEndOfTrackDelta{};
		vector<shared_ptr<mpc::midi::event::MidiEvent>> mEvents{};

	public:
		static MidiTrack* createTempoTrack();

	private:
		void readTrackData(vector<char>& data);

	public:
		vector<weak_ptr<mpc::midi::event::MidiEvent>> getEvents();
		int getEventCount();
		int getSize();
		int getLengthInTicks();
		int getEndOfTrackDelta();
		void setEndOfTrackDelta(int delta);
		void insertNote(int channel, int pitch, int velocity, int tick, int duration);
		void insertEvent(weak_ptr<mpc::midi::event::MidiEvent> newEvent);
		bool removeEvent(mpc::midi::event::MidiEvent* E);
		void closeTrack();
		void dumpEvents();

	private:
		void recalculateSize();

	public:
		void writeToOutputStream(ostream& stream);

	public:
		MidiTrack();
		MidiTrack(istream& in);
	};
}
