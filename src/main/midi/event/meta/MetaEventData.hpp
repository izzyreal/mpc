#pragma once

#include <midi/util/VariableLengthInt.hpp>

#include <sstream>

namespace mpc::midi::event::meta
{

    class MetaEventData
    {

    public:
        int type = 0;
        mpc::midi::util::VariableLengthInt length;
        std::vector<char> data;

    public:
        MetaEventData(std::stringstream &in);
    };
} // namespace mpc::midi::event::meta
