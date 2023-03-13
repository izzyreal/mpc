#pragma once

#include <cstdint>
#include <string>

namespace ctoot::control {

    class ControlLaw
    {

    public:
        virtual int intValue(float userVal);

        virtual std::string getUnits();

    };
}
