#pragma once

#include <ostream>
#include <istream>
#include <memory>
#include <vector>
#include <fstream>

namespace mpc::file::mid::event
{
    class MidiEvent;
}

namespace mpc::file::mid
{
    class MidiTrack
    {

    private:
        static const bool VERBOSE = false;
        static std::vector<char> IDENTIFIER;

    private:
        int mSize;
        bool mSizeNeedsRecalculating;
        bool mClosed;
        int mEndOfTrackDelta;
        std::vector<std::shared_ptr<mpc::file::mid::event::MidiEvent>> mEvents;

    public:
        static MidiTrack *createTempoTrack();

    private:
        void readTrackData(std::vector<char> &data);

    public:
        std::vector<std::weak_ptr<mpc::file::mid::event::MidiEvent>> getEvents();
        int getEventCount();
        int getSize();
        int getLengthInTicks();
        int getEndOfTrackDelta();
        void setEndOfTrackDelta(int delta);
        void insertNote(int channel, int pitch, int velocity, int tick,
                        int duration);
        void insertEvent(std::weak_ptr<mpc::file::mid::event::MidiEvent> newEvent);
        bool removeEvent(mpc::file::mid::event::MidiEvent *E);
        void closeTrack();

    private:
        void recalculateSize();

    public:
        void writeToOutputStream(std::shared_ptr<std::ostream>);

    public:
        MidiTrack();
        MidiTrack(std::shared_ptr<std::istream>);
    };
} // namespace mpc::file::mid
