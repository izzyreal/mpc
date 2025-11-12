#pragma once

#include "engine/control/CompoundControlChain.hpp"

#include <vector>

namespace mpc::engine::audio::core
{

    class AudioControlsChain : public control::CompoundControlChain
    {

        std::string sourceLabel;
        std::string sourceLocation;

    public:
        AudioControlsChain(int id, const std::string &name);
    };
} // namespace mpc::engine::audio::core
