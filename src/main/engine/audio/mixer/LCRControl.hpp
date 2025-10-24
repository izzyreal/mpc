#pragma once

#include <engine/control/LawControl.hpp>
#include <vector>
#include <engine/control/ControlLaw.hpp>

namespace mpc::engine::audio::mixer
{

    class LCRControl
        : public mpc::engine::control::LawControl
    {

    public:
        virtual float getLeft() = 0;

        virtual float getRight() = 0;

    public:
        LCRControl(std::string name, std::shared_ptr<mpc::engine::control::ControlLaw> law,
                   float initialValue);
    };

} // namespace mpc::engine::audio::mixer
