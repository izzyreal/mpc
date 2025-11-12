#pragma once

#include "engine/audio/core/AudioControls.hpp"

#include <string>

namespace mpc::engine::audio::mixer
{

    class BusControls : public core::AudioControls
    {

    public:
        BusControls(int id, const std::string &name);
    };

} // namespace mpc::engine::audio::mixer
