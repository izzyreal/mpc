#pragma once

#include "engine/audio/mixer/LCRControl.hpp"

namespace mpc::engine::audio::mixer
{

    class PanControl : public LCRControl
    {

    private:
        float left;
        float right;

    public:
        float getLeft() override;

        float getRight() override;

        void setValue(float value) override;

    public:
        PanControl();
    };

} // namespace mpc::engine::audio::mixer
