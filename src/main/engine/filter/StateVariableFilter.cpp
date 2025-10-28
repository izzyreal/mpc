#include "StateVariableFilter.hpp"
#include "StateVariableFilterElement.hpp"

using namespace mpc::engine::filter;

StateVariableFilter::StateVariableFilter(FilterControls *variables)
{
    element = new StateVariableFilterElement();
    vars = variables;
}

float StateVariableFilter::update()
{
    res = vars->getResonance();
    element->mix = 0;
    element->bp = false;
    return vars->getCutoff();
}

float StateVariableFilter::filter(float sample, float f)
{
    float min = 0.24f < f * 0.25f ? 0.24f : f * 0.25f;
    auto f1 = 2.0f * sin(static_cast<float>(M_PI * min));
    float v1 = 1.9f;
    float v2 = 2.0f / f1 - f1 * 0.5f;
    min = v1 < v2 ? v1 : v2;
    v1 = res;
    v2 = min;
    min = v1 < v2 ? v1 : v2;
    return element->filter(sample, f1, min);
}

void StateVariableFilter::setSampleRate(int rate)
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
