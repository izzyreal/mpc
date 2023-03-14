#pragma once

#include <engine/audio/mixer/LCRControl.hpp>

namespace mpc::engine::audio::mixer {
    class BalanceControl : public LCRControl
    {
    private:
        float left{1};
        float right{1};

    public:
        float getLeft() override;

        float getRight() override;

        void setValue(float value) override;

    public:
        BalanceControl();

        virtual ~BalanceControl();

    };

}
