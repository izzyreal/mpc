#pragma once

#include "Mpc.hpp"

namespace mpc
{
    class TestMpc
    {
    public:
        static void resetTestDataRoot(Mpc &mpc);
        static void initializeTestMpc(Mpc &mpc);
        static void initializeTestMpcWithoutMidiServices(Mpc &mpc);
        static void initializeTestMpcWithoutIoServices(Mpc &mpc);
    };
} // namespace mpc
