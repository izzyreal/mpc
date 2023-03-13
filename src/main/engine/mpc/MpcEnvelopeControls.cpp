#include "MpcEnvelopeControls.hpp"

#include <engine/control/Control.hpp>
#include <engine/control/LinearLaw.hpp>
#include <engine/control/LogLaw.hpp>

#include <cfloat>
#include <cmath>

using namespace ctoot::mpc;
using namespace std;

MpcEnvelopeControls::MpcEnvelopeControls(int id, string name, int idOffset)
	: CompoundControl(id, name)
{
	this->idOffset = idOffset;
	createControls();
	deriveSampleRateIndependentVariables();
	deriveSampleRateDependentVariables();
}

void MpcEnvelopeControls::derive(ctoot::control::Control* c)
{
	switch (c->getId() - idOffset) {
	case ATTACK:
		attack = deriveAttack();
		break;
	case HOLD:
		hold = deriveHold();
		break;
	case DECAY:
		decay = deriveDecay();
		break;
	}
}

void MpcEnvelopeControls::createControls()
{
	attackControl = createAttackControl(0.0f);
	holdControl = createHoldControl(0.0f);
	decayControl = createDecayControl(0.0f);

	add(unique_ptr<Control>(attackControl));
	add(unique_ptr<Control>(holdControl));
	add(unique_ptr<Control>(decayControl));
}

void MpcEnvelopeControls::deriveSampleRateIndependentVariables()
{
    hold = deriveHold();
}

void MpcEnvelopeControls::deriveSampleRateDependentVariables()
{
    attack = deriveAttack();
    decay = deriveDecay();
}

float MpcEnvelopeControls::deriveHold()
{
    return holdControl->getValue();
}

float MpcEnvelopeControls::LOG_0_01_ = static_cast< float >(log(0.01));

float MpcEnvelopeControls::deriveTimeFactor(float milliseconds)
{
    double ns = milliseconds * sampleRate / 1000;
    double k = LOG_0_01_ / ns;
    return static_cast< float >(1.0f - exp(k));
}

float MpcEnvelopeControls::deriveAttack()
{
    return deriveTimeFactor(attackControl->getValue());
}

float MpcEnvelopeControls::deriveDecay()
{
    return deriveTimeFactor(decayControl->getValue());
}

shared_ptr<ctoot::control::ControlLaw> MpcEnvelopeControls::ATTACK_LAW() {
	static shared_ptr<ctoot::control::LogLaw> res = make_shared<ctoot::control::LogLaw>(0.0000001f, 3000.0f * 4.7f, "ms");
	return res;
}

shared_ptr<ctoot::control::ControlLaw> MpcEnvelopeControls::DECAY_LAW() {
	static shared_ptr<ctoot::control::LogLaw> res = make_shared<ctoot::control::LogLaw>(0.0000001f, 2600.0f * 4.7f, "ms");
	return res;
}

shared_ptr<ctoot::control::ControlLaw> MpcEnvelopeControls::HOLD_LAW() {
	static shared_ptr<ctoot::control::LinearLaw> res = make_shared<ctoot::control::LinearLaw>(0.0f, FLT_MAX, "samples");
	return res;
}

ctoot::control::LawControl* MpcEnvelopeControls::createAttackControl(float init)
{
	return new ctoot::control::LawControl(ATTACK + idOffset, "Attack", ATTACK_LAW(), 0.1f, init);
}

ctoot::control::LawControl* MpcEnvelopeControls::createHoldControl(float init)
{
	return new ctoot::control::LawControl(HOLD + idOffset, "Hold", HOLD_LAW(), 0.1f, init);
}

ctoot::control::LawControl* MpcEnvelopeControls::createDecayControl(float init)
{
	return new ctoot::control::LawControl(DECAY + idOffset, "Decay", DECAY_LAW(), 0.1f, init);
}

float MpcEnvelopeControls::getAttackCoeff()
{
    return attack;
}

float MpcEnvelopeControls::getHold()
{
    return hold;
}

float MpcEnvelopeControls::getDecayCoeff()
{
    return decay;
}

void MpcEnvelopeControls::setSampleRate(int rate) {
	if (sampleRate != rate) {
		sampleRate = rate;
		deriveSampleRateDependentVariables();
	}
}
