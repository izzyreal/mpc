#pragma once

#include <engine/audio/fader/FaderControl.hpp>

namespace ctoot::audio::mixer {

    class GainControl
            : public audio::fader::FaderControl
    {

    public:
        void setValue(float value) override;

    public:
        GainControl(bool muted);
    };

}
