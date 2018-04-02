#include "MpcEnvelopeControls.hpp"

#include <control/Control.hpp>
#include <control/FloatControl.hpp>
#include <control/LinearLaw.hpp>
#include <control/LogLaw.hpp>
//#include <misc/Localisation.hpp>
#include <synth/modules/envelope/EnvelopeControlIds.hpp>

#include <cfloat>
#include <cmath>

using namespace mpc::ctootextensions;
using namespace std;

MpcEnvelopeControls::MpcEnvelopeControls(int id, int instanceIndex, string name, int idOffset, float timeMultiplier)
	: CompoundControl(id, instanceIndex, name)
{
	this->idOffset = idOffset;
	createControls();
	deriveSampleRateIndependentVariables();
	deriveSampleRateDependentVariables();
}

const int MpcEnvelopeControls::MPC_ENVELOPE_ID;

void MpcEnvelopeControls::derive(ctoot::control::Control* c)
{
	switch (c->getId() - idOffset) {
	case ctoot::synth::modules::envelope::EnvelopeControlIds::ATTACK:
		attack = deriveAttack();
		break;
	case ctoot::synth::modules::envelope::EnvelopeControlIds::HOLD:
		hold = deriveHold();
		break;
	case ctoot::synth::modules::envelope::EnvelopeControlIds::DECAY:
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

float& MpcEnvelopeControls::LOG_0_01()
{
    return LOG_0_01_;
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

weak_ptr<ctoot::control::ControlLaw> MpcEnvelopeControls::ATTACK_LAW() {
	static shared_ptr<ctoot::control::LogLaw> res = make_shared<ctoot::control::LogLaw>(0.0f, 3000.0f * 4.7f, "ms");
	return res;
}

weak_ptr<ctoot::control::ControlLaw> MpcEnvelopeControls::DECAY_LAW() {
	static shared_ptr<ctoot::control::LogLaw> res = make_shared<ctoot::control::LogLaw>(0.0f, 2600.0f * 4.7f, "ms");
	return res;
}

weak_ptr<ctoot::control::ControlLaw> MpcEnvelopeControls::HOLD_LAW() {
	static shared_ptr<ctoot::control::LinearLaw> res = make_shared<ctoot::control::LinearLaw>(0.0f, FLT_MAX, "samples");
	return res;
}

ctoot::control::FloatControl* MpcEnvelopeControls::createAttackControl(float init)
{
	return new ctoot::control::FloatControl(ctoot::synth::modules::envelope::EnvelopeControlIds::ATTACK + idOffset, "Attack", ATTACK_LAW(), 0.1f, init);
}

ctoot::control::FloatControl* MpcEnvelopeControls::createHoldControl(float init)
{
	return new ctoot::control::FloatControl(ctoot::synth::modules::envelope::EnvelopeControlIds::HOLD + idOffset, "Hold", HOLD_LAW(), 0.1f, init);
}

ctoot::control::FloatControl* MpcEnvelopeControls::createDecayControl(float init)
{
	return new ctoot::control::FloatControl(ctoot::synth::modules::envelope::EnvelopeControlIds::DECAY + idOffset, "Decay", DECAY_LAW(), 0.1f, init);
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

MpcEnvelopeControls::~MpcEnvelopeControls() {
}
