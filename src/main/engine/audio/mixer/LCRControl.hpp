#pragma once

#include "engine/control/LawControl.hpp"
#include "engine/control/ControlLaw.hpp"

namespace mpc::engine::audio::mixer
{

    class LCRControl : public control::LawControl
    {

    public:
        virtual float getLeft() = 0;

        virtual float getRight() = 0;

        LCRControl(const std::string &name,
                   const std::shared_ptr<control::ControlLaw> &law,
                   float initialValue);
    };

} // namespace mpc::engine::audio::mixer
