#pragma once
#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class Controller : public ChannelEvent
    {

    public:
        int getControllerType();
        int getValue();
        void setControllerType(int t);
        void setValue(int v);

        Controller(int tick, int channel, int controllerType, int value);
        Controller(int tick, int delta, int channel, int controllerType,
                   int value);
    };
} // namespace mpc::file::mid::event
