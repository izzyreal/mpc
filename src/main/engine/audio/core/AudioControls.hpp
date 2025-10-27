#pragma once

#include <engine/control/CompoundControl.hpp>
#include <engine/control/BooleanControl.hpp>

namespace mpc::engine::audio::core
{

    class AudioControls : public control::CompoundControl
    {

    public:
        AudioControls(int id, std::string name);
    };
} // namespace mpc::engine::audio::core
