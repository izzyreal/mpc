#pragma once
#include <midi/event/ChannelEvent.hpp>

namespace mpc
{
    namespace midi
    {
        namespace event
        {

            class ChannelAftertouch : public ChannelEvent
            {

            public:
                virtual int getAmount();
                virtual void setAmount(int p);

                ChannelAftertouch(int tick, int channel, int amount);
                ChannelAftertouch(int tick, int delta, int channel, int amount);
            };

        } // namespace event
    } // namespace midi
} // namespace mpc
