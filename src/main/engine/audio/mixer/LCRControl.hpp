#pragma once

#include <engine/control/LawControl.hpp>
#include <vector>
#include <engine/control/ControlLaw.hpp>

namespace ctoot::audio::mixer {

    class LCRControl
            : public ctoot::control::LawControl
    {

    public:
        virtual float getLeft() = 0;

        virtual float getRight() = 0;

    public:
        LCRControl(std::string name, std::shared_ptr<ctoot::control::ControlLaw> law, float precision,
                   float initialValue);

    };

}
