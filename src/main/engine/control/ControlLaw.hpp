#pragma once

#include <cstdint>
#include <string>

namespace mpc::engine::control
{

    class ControlLaw
    {

    public:
        virtual int intValue(float userVal);

        virtual std::string getUnits();
    };
} // namespace mpc::engine::control
