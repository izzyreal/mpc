#include <engine/audio/mixer/PanControl.hpp>
#include <engine/audio/mixer/MixControls.hpp>
#include <engine/control/LinearLaw.hpp>

#include <cmath>

using namespace mpc::engine::audio::mixer;

PanControl::PanControl()
	: LCRControl("Pan", mpc::engine::control::LinearLaw::UNITY(), 0.5f)
{
	left = MixControls::HALF_ROOT_TWO();
	right = MixControls::HALF_ROOT_TWO();
}

float PanControl::getLeft()
{
    return left;
}

float PanControl::getRight()
{
    return right;
}

void PanControl::setValue(float value)
{
    left = static_cast< float >(cos(M_PI * 0.5 * value));
    right = static_cast< float >(sin(M_PI * 0.5 * value));
	LCRControl::setValue(value);
}
