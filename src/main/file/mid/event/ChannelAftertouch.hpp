#pragma once
#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class ChannelAftertouch : public ChannelEvent
    {
    public:
        int getAmount();
        void setAmount(int p);

        ChannelAftertouch(int tick, int channel, int amount);
        ChannelAftertouch(int tick, int delta, int channel, int amount);
    };
} // namespace mpc::file::mid::event
