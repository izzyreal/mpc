#include <engine/audio/mixer/MixProcess.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;
using namespace std;

MixProcess::MixProcess(shared_ptr<AudioMixerStrip> strip, shared_ptr<MixVariables> vars)
{
	if (!strip) {
		return;
	}
	routedStrip = strip;
	this->vars = vars;
	smoothingFactor = vars->getSmoothingFactor();
	channelGains = vector<float>(2);
	smoothedChannelGains = vector<float>(2);
}

AudioMixerStrip* MixProcess::getRoutedStrip()
{
    return routedStrip.get();
}

int MixProcess::processAudio(AudioBuffer* buffer)
{
    auto lVars = vars;
	if (!lVars->isEnabled() && lVars->isMaster()) {
		buffer->makeSilence();
	}
	else if (lVars->isEnabled()) {
//		gain = vars->getGain();
		//if (gain > 0.0f || vars->isMaster()) {
			lVars->getChannelGains(&channelGains);
			for (auto c = 0; c < channelGains.size(); c++) {
				smoothedChannelGains[c] += smoothingFactor * (channelGains[c] - smoothedChannelGains[c]);
			}
			getRoutedStrip()->mix(buffer, smoothedChannelGains);
		//}
	}
	return AUDIO_OK;
}
