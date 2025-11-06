#include "engine/control/AbstractLaw.hpp"

using namespace mpc::engine::control;
using namespace std;

AbstractLaw::AbstractLaw(float min, float max, const string &units)
{
    this->min = min;
    this->max = max;
    this->units = units;
}

const int AbstractLaw::resolution;

string AbstractLaw::getUnits()
{
    return units;
}
