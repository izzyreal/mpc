#include "FilterControls.hpp"

#include <engine/control/Control.hpp>
#include <engine/control/LawControl.hpp>
#include <engine/control/LinearLaw.hpp>

using namespace mpc::engine::filter;
using namespace std;

FilterControls::FilterControls(int id, string name, int idOffset)
	: CompoundControl(id, name)
{
	this->idOffset = idOffset;
	createControls();
	deriveSampleRateIndependentVariables();
}

shared_ptr<mpc::engine::control::ControlLaw> FilterControls::SEMITONE_LAW() {
	static shared_ptr<mpc::engine::control::ControlLaw> res = make_shared<mpc::engine::control::LinearLaw>(-48, 96, "semitones");
	return res;
}

void FilterControls::derive(mpc::engine::control::Control* c)
{
	switch (c->getId() - idOffset) {
	case FREQUENCY:
		cutoff = deriveCutoff();
		break;
	case RESONANCE:
		resonance = deriveResonance();
		break;
	}
}

void FilterControls::createControls()
{
	cutoffControl = createCutoffControl();
	add(shared_ptr<Control>(cutoffControl));
	resonanceControl = createResonanceControl();
	add(shared_ptr<Control>(resonanceControl));
}

void FilterControls::deriveSampleRateIndependentVariables()
{
	resonance = deriveResonance();
	cutoff = deriveCutoff();
}

float FilterControls::deriveResonance()
{
	return resonanceControl->getValue();
}

float FilterControls::deriveCutoff()
{
	return cutoffControl->getValue();
}

mpc::engine::control::LawControl* FilterControls::createCutoffControl()
{
	return new mpc::engine::control::LawControl(FREQUENCY + idOffset, "Cutoff", SEMITONE_LAW(), 1.0f, 0.0f);
}

mpc::engine::control::LawControl* FilterControls::createResonanceControl()
{
	return new mpc::engine::control::LawControl(RESONANCE + idOffset, "Resonance", mpc::engine::control::LinearLaw::UNITY(), 0.01f, 0.25f);
}

float FilterControls::getCutoff()
{
	return cutoff;
}

float FilterControls::getResonance()
{
	return resonance;
}
