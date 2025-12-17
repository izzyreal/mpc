#include "StateVariableFilterElement.hpp"

using namespace mpc::engine::filter;

StateVariableFilterElement::StateVariableFilterElement()
{
    resetState();
}

float StateVariableFilterElement::filter(const float in, const float freq,
                                         const float damp)
{
    const auto i1 = (prev + in) * 0.5f;

    prev = in;

    low += freq * band;
    high = i1 - damp * band - low;
    band += freq * high;

    low += freq * band;
    high = in - damp * band - low;
    band += freq * high;

    return low;
}

void StateVariableFilterElement::resetState()
{
    prev = 0.f;
    low = 0.f;
    high = 0.f;
    band = 0.f;
    notch = 0.f;
}
