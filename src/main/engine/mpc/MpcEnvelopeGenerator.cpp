#include <engine/mpc/MpcEnvelopeGenerator.hpp>

#include <engine/mpc/MpcEnvelopeControls.hpp>

using namespace ctoot::mpc;

MpcEnvelopeGenerator::MpcEnvelopeGenerator(MpcEnvelopeControls* vars) 
{
	state = ATTACK;
	envelope = 0.0f;
	holdCounter = 0;
	this->vars = vars;
}

float MpcEnvelopeGenerator::getEnvelope(bool decay)
{
	if (decay && state != COMPLETE)
		state = DECAY;

	if (state == HOLD && holdCounter >= vars->getHold())
		state = DECAY;

	{
		auto v = state;
        if (v == ATTACK) {
			envelope += vars->getAttackCoeff();
			if (envelope > 0.99f) {
				state = HOLD;
			}
			goto end_switch0;
		}
        if (v == HOLD) {
			holdCounter++;
			goto end_switch0;
		}
        if (v == DECAY) {
			envelope -= vars->getDecayCoeff();
			if (envelope < 0.001f) {
				envelope = 0.0f;
				state = COMPLETE;
			}
			goto end_switch0;
		}
        if (v == COMPLETE) {
			goto end_switch0;
		}
	end_switch0:;
	}

	return envelope;
}

bool MpcEnvelopeGenerator::isComplete()
{
    return state == COMPLETE;
}

void MpcEnvelopeGenerator::reset()
{
    state = ATTACK;
    holdCounter = 0;
    envelope = 0.0f;
}

MpcEnvelopeGenerator::~MpcEnvelopeGenerator() {
}
