#include "MpcFaderControl.hpp"

#include <engine/audio/mixer/MixControlIds.hpp>
#include <engine/control/LinearLaw.hpp>

using namespace ctoot::mpc;

MpcFaderControl::MpcFaderControl(bool muted) 
	: FaderControl(ctoot::audio::mixer::MixControlIds::GAIN, MPC_FADER_LAW(), muted ? 0 : 100)
{
	gain = muted ? 0.0f : 1.0f;
}

std::shared_ptr<ctoot::control::ControlLaw> MpcFaderControl::MPC_FADER_LAW() {
	static auto res = std::make_shared<ctoot::control::LinearLaw>(0, 100, "");
	return res;
}

void MpcFaderControl::setValue(float value)
{
    gain = value * 0.01f;
    FaderControl::setValue(value);
}
