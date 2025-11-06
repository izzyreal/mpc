#pragma once

#include "engine/control/Control.hpp"
#include "engine/control/ControlLaw.hpp"

#include <memory>

namespace mpc::engine::control
{

    class LawControl : public Control
    {

    private:
        std::shared_ptr<ControlLaw> law;
        float value{0};

    public:
        virtual float getValue();

        virtual void setValue(float value);

    public:
        std::string getValueString() override;

    public:
        LawControl(int id, std::string name, std::shared_ptr<ControlLaw> law,
                   float initialValue);
    };

} // namespace mpc::engine::control
