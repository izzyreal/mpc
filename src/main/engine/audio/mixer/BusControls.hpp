#pragma once

#include "engine/audio/core/AudioControls.hpp"

#include <string>

namespace mpc::engine::audio::mixer
{

    class BusControls : public mpc::engine::audio::core::AudioControls
    {

    public:
        BusControls(int id, std::string name);
    };

} // namespace mpc::engine::audio::mixer
