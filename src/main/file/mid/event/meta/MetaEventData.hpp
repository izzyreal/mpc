#pragma once

#include "file/mid/util/VariableLengthInt.hpp"

#include <sstream>

namespace mpc::file::mid::event::meta
{

    class MetaEventData
    {

    public:
        int type = 0;
        mpc::file::mid::util::VariableLengthInt length;
        std::vector<char> data;

    public:
        MetaEventData(std::stringstream &in);
    };
} // namespace mpc::file::mid::event::meta
