#include "StateVariableFilter.hpp"
#include "StateVariableFilterElement.hpp"

#include "mpc_types.hpp"

#include <cmath>

using namespace mpc::engine::filter;

StateVariableFilter::StateVariableFilter(FilterControls *variables)
{
    element = new StateVariableFilterElement();
    vars = variables;
}

float StateVariableFilter::filter(const float sample, const float cutoff,
                                  const float resonance) const
{
    float min = 0.24f < cutoff * 0.25f ? 0.24f : cutoff * 0.25f;
    const auto f1 = 2.0f * sin(static_cast<float>(math::pi * min));
    float v1 = 1.9f;
    float v2 = 2.0f / f1 - f1 * 0.5f;
    min = v1 < v2 ? v1 : v2;
    v1 = resonance;
    v2 = min;
    min = v1 < v2 ? v1 : v2;
    return element->filter(sample, f1, min);
}

void StateVariableFilter::resetElementState() const
{
    element->resetState();
}

void StateVariableFilter::setSampleRate(const int rate)
{
    fs = rate;
}

StateVariableFilter::~StateVariableFilter()
{
    if (element != nullptr)
    {
        delete element;
    }
}
