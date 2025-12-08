#pragma once

#include <string>

namespace mpc::sequencer
{
    struct SequenceMetaInfo
    {
        bool used;
        std::string name;
    };
} // namespace mpc::sequencer