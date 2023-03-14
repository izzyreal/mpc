#include "FaderControl.hpp"

#include <engine/audio/mixer/MixControlIds.hpp>
#include <engine/control/LinearLaw.hpp>

using namespace ctoot::mpc;
using namespace ctoot::control;

FaderControl::FaderControl()
	: LawControl(ctoot::audio::mixer::MixControlIds::GAIN, "Level", MPC_FADER_LAW(), 0.1, 100)
{
	gain = 1.0f;
}

std::shared_ptr<ControlLaw> FaderControl::MPC_FADER_LAW() {
	static auto res = std::make_shared<LinearLaw>(0, 100, "");
	return res;
}

void FaderControl::setValue(float value)
{
    gain = value * 0.01f;
    LawControl::setValue(value);
}

float FaderControl::getGain()
{
	return gain;
}

