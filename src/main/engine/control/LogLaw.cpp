#include "engine/control/LogLaw.hpp"

#include "engine/control/ControlLaw.hpp"

#include <cassert>

using namespace mpc::engine::control;
using namespace std;

LogLaw::LogLaw(float min, float max, const string &units)
    : AbstractLaw(min, max, units)
{
    assert(min != 0.f);
    assert(max != 0.f);
    logMin = log10(min);
    logMax = log10(max);
    logSpan = logMax - logMin;
}

int LogLaw::intValue(float userVal)
{
    if (userVal == 0)
    {
        userVal = 1;
    }
    return static_cast<int>(0.5f + (resolution - 1) *
                                       (log10(userVal) - logMin) / logSpan);
}
