#include "engine/control/LinearLaw.hpp"

using namespace mpc::engine::control;
using namespace std;

LinearLaw::LinearLaw(float min, float max, const string &units)
    : AbstractLaw(min, max, units)
{
}

shared_ptr<LinearLaw> LinearLaw::UNITY()
{
    static shared_ptr<LinearLaw> res = make_shared<LinearLaw>(0.f, 1.f, "");
    return res;
}

int LinearLaw::intValue(float v)
{
    return static_cast<int>((resolution - 1) * (v - min) / (max - min));
}
