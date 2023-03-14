#pragma once

#include <engine/control/LawControl.hpp>

#include <memory>

namespace ctoot::mpc {

    class MpcFaderControl : public ctoot::control::LawControl
    {

    private:
        static std::shared_ptr<ctoot::control::ControlLaw> MPC_FADER_LAW();

    protected:
        float gain{0};

        static float ATTENUATION_CUTOFF();

    public:
        float getGain();

        void setValue(float value) override;

        MpcFaderControl();

    };

}
