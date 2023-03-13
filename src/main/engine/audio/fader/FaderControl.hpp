#pragma once

#include <engine/control/LawControl.hpp>
#include <engine/control/ControlLaw.hpp>

#include <cstdint>
#include <memory>

namespace ctoot::audio::fader {

    class FaderLaw;

    class FaderControl
            : public ctoot::control::LawControl
    {

    protected:
        float gain{0};

    public:
        float getGain();

    public:

        static std::shared_ptr<FaderLaw> BROADCAST();

        static float ATTENUATION_CUTOFF();

        static std::shared_ptr<FaderLaw> defaultLaw();

    public:
        FaderControl(int id, std::shared_ptr<ctoot::control::ControlLaw> law, float initialValue);

        virtual ~FaderControl();

    };
}
