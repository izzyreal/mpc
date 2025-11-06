#pragma once

#include "engine/control/AbstractLaw.hpp"
#include <string>

#include <memory>

namespace mpc::engine::control
{

    class LinearLaw : public AbstractLaw
    {

    public:
        int intValue(float v) override;

        LinearLaw(float min, float max, std::string units);

    public:
        static std::shared_ptr<LinearLaw> UNITY();
    };
} // namespace mpc::engine::control
