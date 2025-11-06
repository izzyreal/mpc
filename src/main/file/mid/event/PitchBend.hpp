#pragma once
#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class PitchBend : public ChannelEvent
    {

    public:
        int getLeastSignificantBits() const;
        int getMostSignificantBits() const;
        int getBendAmount() const;
        void setLeastSignificantBits(int p);
        void setMostSignificantBits(int p);
        void setBendAmount(int amount);

        PitchBend(int tick, int channel, int lsb, int msb);
        PitchBend(int tick, int delta, int channel, int lsb, int msb);
    };
} // namespace mpc::file::mid::event
