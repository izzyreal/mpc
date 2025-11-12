#include "StateVariableFilterControls.hpp"
#include "engine/control/Control.hpp"
#include "engine/control/LinearLaw.hpp"

using namespace mpc::engine::filter;
using namespace std;

StateVariableFilterControls::StateVariableFilterControls(const string &name,
                                                         int idOffset)
    : FilterControls(34, name, idOffset)
{
}

void StateVariableFilterControls::derive(Control *c)
{
    FilterControls::derive(c);
}

void StateVariableFilterControls::deriveSampleRateIndependentVariables()
{
    FilterControls::deriveSampleRateIndependentVariables();
}

float StateVariableFilterControls::deriveResonance()
{
    return static_cast<float>(
        2 * (1.0f - pow(FilterControls::deriveResonance(), 0.25)));
}
