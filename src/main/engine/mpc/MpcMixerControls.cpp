#include <engine/mpc/MpcMixerControls.hpp>
#include <engine/mpc/MpcFaderControl.hpp>
#include <engine/audio/fader/FaderControl.hpp>
#include <engine/audio/fader/FaderLaw.hpp>

using namespace ctoot::audio::fader;
using namespace ctoot::mpc;
using namespace std;

MpcMixerControls::MpcMixerControls(string name, float smoothingFactor) 
	: MixerControls(name, smoothingFactor)
{
}

FaderControl* MpcMixerControls::createFaderControl(bool muted)
{
    return new MpcFaderControl(muted);
}
