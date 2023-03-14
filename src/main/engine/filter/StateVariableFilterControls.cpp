#include "StateVariableFilterControls.hpp"
#include <engine/control/Control.hpp>
#include <engine/control/LawControl.hpp>
#include <engine/control/LinearLaw.hpp>

#include <cmath>

using namespace mpc::engine::filter;
using namespace std;

StateVariableFilterControls::StateVariableFilterControls(string name, int idOffset)
	: FilterControls(34, name, idOffset)
{
}

void StateVariableFilterControls::derive(mpc::engine::control::Control* c)
{
	FilterControls::derive(c);
}

void StateVariableFilterControls::deriveSampleRateIndependentVariables()
{
    FilterControls::deriveSampleRateIndependentVariables();
}

float StateVariableFilterControls::deriveResonance()
{
    return static_cast< float >(2 * (1.0f - pow(FilterControls::deriveResonance(), 0.25)));
}
