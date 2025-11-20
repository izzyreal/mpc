#pragma once

#include "performance/Sequence.hpp"

namespace mpc::file::all
{

    class AllControlChangeEvent
    {

        const static int CONTROLLER_OFFSET = 5;
        const static int AMOUNT_OFFSET = 6;

    public:
        static performance::Event bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(const performance::Event &);
    };
} // namespace mpc::file::all
