#pragma once

#include <engine/control/AbstractLaw.hpp>

namespace mpc::engine::control
{

    class LogLaw
        : public AbstractLaw
    {

    private:
        double logMin, logMax;
        double logSpan;

    public:
        int intValue(float userVal) override;

    public:
        LogLaw(float min, float max, std::string units);
    };

} // namespace mpc::engine::control
