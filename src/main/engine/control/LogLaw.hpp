#pragma once

#include "engine/control/AbstractLaw.hpp"

namespace mpc::engine::control
{

    class LogLaw : public AbstractLaw
    {

        double logMin, logMax;
        double logSpan;

    public:
        int intValue(float userVal) override;

        LogLaw(float min, float max, const std::string &units);
    };

} // namespace mpc::engine::control
