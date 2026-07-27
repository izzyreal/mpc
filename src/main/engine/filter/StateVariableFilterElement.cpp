#include "StateVariableFilterElement.hpp"

using namespace mpc::engine::filter;

float StateVariableFilterElement::filter(const float input,
                                         const double coefficient,
                                         const double damping)
{
    const auto high = input - low - damping * band;
    band += coefficient * high;
    low += coefficient * band;
    return static_cast<float>(low);
}

void StateVariableFilterElement::resetState()
{
    low = 0;
    band = 0;
}
