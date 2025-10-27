#pragma once

#include "Mpc.hpp"

namespace mpc::engine::midi
{
    class ShortMessage;
}

namespace mpc::audiomidi
{

    class VmpcMidiControlMode
    {
    public:
        int previousDataWheelValue = -1;

        void processMidiInputEvent(mpc::Mpc &mpc,
                                   mpc::engine::midi::ShortMessage *msg);
    };
} // namespace mpc::audiomidi