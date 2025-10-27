#pragma once

#include "engine/control/LawControl.hpp"

#include <memory>

namespace mpc::engine
{

    class FaderControl : public mpc::engine::control::LawControl
    {

    private:
        static std::shared_ptr<mpc::engine::control::ControlLaw>
        MPC_FADER_LAW();

    protected:
        float gain = 0.0f;

    public:
        float getGain();

        void setValue(float value) override;

        FaderControl();
    };

} // namespace mpc::engine
