#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

#include <ostream>

namespace mpc::file::mid::event::meta
{
    class EndOfTrack : public MetaEvent
    {

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

        int compareTo(MidiEvent *other) const;

        EndOfTrack(int tick, int delta);
    };
} // namespace mpc::file::mid::event::meta
