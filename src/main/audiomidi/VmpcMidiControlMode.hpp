#pragma once

#include "Mpc.hpp"

namespace ctoot::midi::core { class ShortMessage; }

namespace mpc::audiomidi {

    class VmpcMidiControlMode
    {
    public:
        int previousDataWheelValue = -1;

        void processMidiInputEvent(mpc::Mpc &mpc, ctoot::midi::core::ShortMessage *msg);
    };
}