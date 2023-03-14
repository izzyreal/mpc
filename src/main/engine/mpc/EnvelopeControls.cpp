#include "EnvelopeControls.hpp"

#include <engine/control/Control.hpp>
#include <engine/control/LinearLaw.hpp>
#include <engine/control/LogLaw.hpp>

#include <cfloat>
#include <cmath>

using namespace ctoot::mpc;
using namespace ctoot::control;
using namespace std;

EnvelopeControls::EnvelopeControls(int id, string name, int idOffset)
	: CompoundControl(id, name)
{
	this->idOffset = idOffset;
	createControls();
	deriveSampleRateIndependentVariables();
	deriveSampleRateDependentVariables();
}

void EnvelopeControls::derive(Control* c)
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

void EnvelopeControls::createControls()
{
	attackControl = createAttackControl(0.0f);
	holdControl = createHoldControl(0.0f);
	decayControl = createDecayControl(0.0f);

	add(unique_ptr<Control>(attackControl));
	add(unique_ptr<Control>(holdControl));
	add(unique_ptr<Control>(decayControl));
}

void EnvelopeControls::deriveSampleRateIndependentVariables()
{
    hold = deriveHold();
}

void EnvelopeControls::deriveSampleRateDependentVariables()
{
    attack = deriveAttack();
    decay = deriveDecay();
}

float EnvelopeControls::deriveHold()
{
    return holdControl->getValue();
}

float EnvelopeControls::LOG_0_01_ = static_cast< float >(log(0.01));

float EnvelopeControls::deriveTimeFactor(float milliseconds)
{
    double ns = milliseconds * sampleRate / 1000;
    double k = LOG_0_01_ / ns;
    return static_cast< float >(1.0f - exp(k));
}

float EnvelopeControls::deriveAttack()
{
    return deriveTimeFactor(attackControl->getValue());
}

float EnvelopeControls::deriveDecay()
{
    return deriveTimeFactor(decayControl->getValue());
}

shared_ptr<ControlLaw> EnvelopeControls::ATTACK_LAW() {
	static shared_ptr<LogLaw> res = make_shared<LogLaw>(0.0000001f, 3000.0f * 4.7f, "ms");
	return res;
}

shared_ptr<ControlLaw> EnvelopeControls::DECAY_LAW() {
	static shared_ptr<LogLaw> res = make_shared<LogLaw>(0.0000001f, 2600.0f * 4.7f, "ms");
	return res;
}

shared_ptr<ControlLaw> EnvelopeControls::HOLD_LAW() {
	static shared_ptr<LinearLaw> res = make_shared<LinearLaw>(0.0f, FLT_MAX, "samples");
	return res;
}

LawControl* EnvelopeControls::createAttackControl(float init)
{
	return new LawControl(ATTACK + idOffset, "Attack", ATTACK_LAW(), 0.1f, init);
}

LawControl* EnvelopeControls::createHoldControl(float init)
{
	return new LawControl(HOLD + idOffset, "Hold", HOLD_LAW(), 0.1f, init);
}

LawControl* EnvelopeControls::createDecayControl(float init)
{
	return new LawControl(DECAY + idOffset, "Decay", DECAY_LAW(), 0.1f, init);
}

float EnvelopeControls::getAttackCoeff()
{
    return attack;
}

float EnvelopeControls::getHold()
{
    return hold;
}

float EnvelopeControls::getDecayCoeff()
{
    return decay;
}

void EnvelopeControls::setSampleRate(int rate) {
	if (sampleRate != rate) {
		sampleRate = rate;
		deriveSampleRateDependentVariables();
	}
}
