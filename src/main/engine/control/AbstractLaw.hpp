#pragma once

#include "engine/control/ControlLaw.hpp"

namespace mpc::engine::control
{

    class AbstractLaw : public ControlLaw
    {

    protected:
        static const int resolution{16384};
        float min;
        float max;
        std::string units;

    public:
        std::string getUnits() override;

    public:
        AbstractLaw(float min, float max, const std::string &units);
    };

} // namespace mpc::engine::control
