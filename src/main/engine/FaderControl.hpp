#pragma once

#include "engine/control/LawControl.hpp"

#include <memory>
#include <atomic>

namespace mpc::engine
{
    class FaderControl final : public control::LawControl
    {
        static std::shared_ptr<control::ControlLaw> MPC_FADER_LAW();

    protected:
        std::atomic<float> gain{0.0f};

    public:
        float getGain() const;

        void setValue(float value) override;

        FaderControl();
    };

} // namespace mpc::engine
