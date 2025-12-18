#pragma once

#include "engine/control/Control.hpp"
#include "engine/control/ControlLaw.hpp"

#include <memory>
#include <atomic>

namespace mpc::engine::control
{

    class LawControl : public Control
    {
        std::shared_ptr<ControlLaw> law;
        std::atomic<float> value{0};

    public:
        virtual float getValue();

        virtual void setValue(float value);

        std::string getValueString() override;

        LawControl(int id, const std::string &name,
                   const std::shared_ptr<ControlLaw> &law, float initialValue);
    };

} // namespace mpc::engine::control
