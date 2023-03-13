#include <engine/audio/mixer/MixProcess.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>

using namespace ctoot::audio::mixer;
using namespace ctoot::audio::core;
using namespace std;

MixProcess::MixProcess(shared_ptr<AudioMixerStrip> strip, shared_ptr<MixVariables> vars)
{
	if (!strip) {
		return;
	}
	routedStrip = strip;
	this->vars = vars;
	auto format = vars->getChannelFormat();
	smoothingFactor = vars->getSmoothingFactor();
	channelGains = vector<float>(format->getCount());
	smoothedChannelGains = vector<float>(format->getCount());
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
