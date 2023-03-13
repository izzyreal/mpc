#include <engine/audio/mixer/MainMixProcess.hpp>
#include <engine/audio/mixer/AudioMixer.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>
#include <engine/audio/mixer/MixVariables.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>

using namespace std;
using namespace ctoot::audio::mixer;

MainMixProcess::MainMixProcess(shared_ptr<AudioMixerStrip> strip, shared_ptr<MixVariables> vars, AudioMixer* mixer)
	: MixProcess(strip, vars)
{
	auto mmc = dynamic_pointer_cast<MainMixControls>(vars);
	routeControl = mmc->getRouteControl();
	if (routeControl != nullptr) {
		routedStrip = mixer->getStripImpl(routeControl->getValueString());
	}
}

AudioMixerStrip* MainMixProcess::getRoutedStrip()
{
	if (routeControl != nullptr && nextRoutedStrip != nullptr) {
		routedStrip = nextRoutedStrip;
		nextRoutedStrip = shared_ptr<AudioMixerStrip>();
	}
	return MixProcess::getRoutedStrip();
}
