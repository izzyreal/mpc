#include "FaderControl.hpp"

#include <engine/audio/fader/FaderLaw.hpp>

using namespace std;
using namespace ctoot::audio::fader;

FaderControl::FaderControl(int id, shared_ptr<ctoot::control::ControlLaw> law, float initialValue)
	: LawControl(id, "Level", law, 0.1f, initialValue)
{
}

float FaderControl::getGain()
{
    return gain;
}

shared_ptr<FaderLaw> FaderControl::BROADCAST()
{
	static shared_ptr<FaderLaw> res = make_shared<FaderLaw>(1024, -30.0f, 15.0f, 0.125f);
	return res;
}

float FaderControl::ATTENUATION_CUTOFF()
{
	return 100.0f;
}

shared_ptr<FaderLaw> FaderControl::defaultLaw()
{
	return FaderControl::BROADCAST();
}

FaderControl::~FaderControl() {

}
