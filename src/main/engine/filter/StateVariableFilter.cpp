#include "StateVariableFilter.hpp"
#include "StateVariableFilterElement.hpp"

#include "engine/Mpc2000XlFilterEnvelope.hpp"

using namespace mpc::engine::filter;

StateVariableFilter::StateVariableFilter()
{
    element = new StateVariableFilterElement();
}

float StateVariableFilter::filter(const float sample,
                                  const double coefficient,
                                  const int resonance) const
{
    return element->filter(
        sample, coefficient,
        mpc::engine::Mpc2000XlFilterEnvelope::resonanceDamping(resonance));
}

void StateVariableFilter::resetElementState() const
{
    element->resetState();
}

StateVariableFilter::~StateVariableFilter()
{
    if (element != nullptr)
    {
        delete element;
    }
}
