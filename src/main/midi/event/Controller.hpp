#pragma once
#include <midi/event/ChannelEvent.hpp>

namespace mpc
{
    namespace midi
    {
        namespace event
        {

            class Controller
                : public ChannelEvent
            {

            public:
                int getControllerType();
                int getValue();
                void setControllerType(int t);
                void setValue(int v);

                Controller(int tick, int channel, int controllerType, int value);
                Controller(int tick, int delta, int channel, int controllerType, int value);
            };

        } // namespace event
    } // namespace midi
} // namespace mpc
