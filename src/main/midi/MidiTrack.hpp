#pragma once

#include <io/InputStream.hpp>
#include <io/OutputStream.hpp>

#include <memory>
#include <vector>

namespace mpc {
	namespace midi {

		namespace event {
			class MidiEvent;
		}

		class MidiTrack
		{

		private:
			static const bool VERBOSE{ false };
			static std::vector<char> IDENTIFIER;
			
		private:
			int mSize{};
			bool mSizeNeedsRecalculating{};
			bool mClosed{};
			int mEndOfTrackDelta{};
			std::vector<std::shared_ptr<mpc::midi::event::MidiEvent>> mEvents{};

		public:
			static MidiTrack* createTempoTrack();

		private:
			void readTrackData(std::vector<char> data);

		public:
			std::vector<std::weak_ptr<mpc::midi::event::MidiEvent>> getEvents();
			int getEventCount();
			int getSize();
			int getLengthInTicks();
			int getEndOfTrackDelta();
			void setEndOfTrackDelta(int delta);
			void insertNote(int channel, int pitch, int velocity, int tick, int duration);
			void insertEvent(std::weak_ptr<mpc::midi::event::MidiEvent> newEvent);
			bool removeEvent(mpc::midi::event::MidiEvent* E);
			void closeTrack();
			void dumpEvents();

		private:
			void recalculateSize();

		public:
			void writeToFile(moduru::io::OutputStream* out);

			MidiTrack();
			MidiTrack(moduru::io::InputStream* in);
		};

	}
}
