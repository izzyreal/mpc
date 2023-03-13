#pragma once

#include <engine/audio/fader/FaderControl.hpp>

#include <memory>

namespace ctoot::mpc {

    class MpcFaderControl
            : public ctoot::audio::fader::FaderControl
    {

    private:
        static std::shared_ptr<ctoot::control::ControlLaw> MPC_FADER_LAW();

    public:
        void setValue(float value) override;

        MpcFaderControl();

    };

}
