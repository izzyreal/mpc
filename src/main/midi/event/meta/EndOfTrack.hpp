#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <ostream>

namespace mpc::midi::event::meta
{
    class EndOfTrack : public MetaEvent
    {

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

    public:
        int compareTo(mpc::midi::event::MidiEvent *other);

    public:
        EndOfTrack(int tick, int delta);
    };
} // namespace mpc::midi::event::meta
