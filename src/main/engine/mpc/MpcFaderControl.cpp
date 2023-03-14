#include "MpcFaderControl.hpp"

#include <engine/audio/mixer/MixControlIds.hpp>
#include <engine/control/LinearLaw.hpp>

using namespace ctoot::mpc;
using namespace ctoot::control;

MpcFaderControl::MpcFaderControl()
	: LawControl(ctoot::audio::mixer::MixControlIds::GAIN, "Level", MPC_FADER_LAW(), 0.1, 100)
{
	gain = 1.0f;
}

std::shared_ptr<ctoot::control::ControlLaw> MpcFaderControl::MPC_FADER_LAW() {
	static auto res = std::make_shared<ctoot::control::LinearLaw>(0, 100, "");
	return res;
}

void MpcFaderControl::setValue(float value)
{
    gain = value * 0.01f;
    LawControl::setValue(value);
}

float MpcFaderControl::getGain()
{
	return gain;
}

float MpcFaderControl::ATTENUATION_CUTOFF()
{
	return 100.0f;
}
