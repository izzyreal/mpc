#include <engine/audio/mixer/GainControl.hpp>
#include <engine/audio/fader/FaderControl.hpp>
#include <engine/audio/fader/FaderLaw.hpp>
#include <engine/audio/mixer/MixControlIds.hpp>
#include <cmath>

using namespace ctoot::audio::mixer;
using namespace ctoot::audio::fader;
using namespace std;

GainControl::GainControl(bool muted) 
	: FaderControl(MixControlIds::GAIN, FaderControl::defaultLaw(), muted ? -FaderControl::ATTENUATION_CUTOFF() : 0.0f)
{
	gain = muted ? 0.0f : 1.0f;
}

void GainControl::setValue(float value)
{
	if (value <= -FaderControl::ATTENUATION_CUTOFF()) {
		gain = 0.0f;
	}
	else {
		gain = (float)(pow(10.0, value * 0.05));
	}
	FaderControl::setValue(value);
}
