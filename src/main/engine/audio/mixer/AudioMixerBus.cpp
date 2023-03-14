#include <engine/audio/mixer/AudioMixerBus.hpp>
#include <engine/audio/mixer/AudioMixer.hpp>

using namespace mpc::engine::audio::core;
using namespace mpc::engine::audio::mixer;
using namespace std;

AudioMixerBus::AudioMixerBus(AudioMixer* mixer, shared_ptr<BusControls> busControls)
{
	auto lBusControls = busControls;
	this->mixer = mixer;
	name = lBusControls->getName();
	channelFormat = lBusControls->getChannelFormat();
	buffer = mixer->createBuffer(name);
	buffer->setChannelFormat(channelFormat);
}

AudioBuffer* AudioMixerBus::getBuffer()
{
    return buffer;
}

string AudioMixerBus::getName()
{
    return name;
}

void AudioMixerBus::silence()
{
    buffer->makeSilence();
}

void AudioMixerBus::write(int nFrames)
{
	if (!output) return;

	if (output) {
		output->processAudio(buffer, nFrames);
	}
	if (meter != nullptr) {
		meter->processAudio(buffer, nFrames);
	}
}

void AudioMixerBus::close()
{
}

AudioMixerBus::~AudioMixerBus() {
	if (meter != nullptr) {
		delete meter;
	}
}
