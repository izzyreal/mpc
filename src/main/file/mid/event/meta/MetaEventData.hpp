#pragma once

#include "file/mid/util/VariableLengthInt.hpp"

#include <sstream>

namespace mpc::file::mid::event::meta
{

    class MetaEventData
    {

    public:
        int type = 0;
        util::VariableLengthInt length;
        std::vector<char> data;

        MetaEventData(std::stringstream &in);
    };
} // namespace mpc::file::mid::event::meta
