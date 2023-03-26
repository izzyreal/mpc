#include <engine/audio/mixer/MixProcess.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;

MixProcess::MixProcess(const std::shared_ptr<AudioMixerStrip>& strip, const std::shared_ptr<MixControls>& mixControls)
{
	if (!strip) {
		return;
	}
	routedStrip = strip;
	this->mixControls = mixControls;
	smoothingFactor = mixControls->getSmoothingFactor();
	channelGains = std::vector<float>(2);
	smoothedChannelGains = std::vector<float>(2);
}

AudioMixerStrip* MixProcess::getRoutedStrip()
{
    return routedStrip.get();
}

int MixProcess::processAudio(AudioBuffer* buffer)
{
	if (!mixControls->isEnabled() && mixControls->isMaster()) {
		buffer->makeSilence();
	}
	else if (mixControls->isEnabled()) {
        mixControls->getChannelGains(&channelGains);
			for (auto c = 0; c < channelGains.size(); c++) {
				smoothedChannelGains[c] += smoothingFactor * (channelGains[c] - smoothedChannelGains[c]);
			}
        getRoutedStrip()->mix(buffer, smoothedChannelGains);
	}
	return AUDIO_OK;
}
