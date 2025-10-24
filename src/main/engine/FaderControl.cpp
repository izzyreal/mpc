#include "FaderControl.hpp"

#include "engine/audio/mixer/MixControlIds.hpp"
#include "engine/control/LinearLaw.hpp"

using namespace mpc::engine;
using namespace mpc::engine::control;

FaderControl::FaderControl()
    : LawControl(mpc::engine::audio::mixer::MixControlIds::GAIN, "Level", MPC_FADER_LAW(), 100.f)
{
}

std::shared_ptr<ControlLaw> FaderControl::MPC_FADER_LAW()
{
    static auto res = std::make_shared<LinearLaw>(0.f, 100.f, "");
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
