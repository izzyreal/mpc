#include <engine/audio/mixer/AudioMixerStrip.hpp>

#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioControls.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include <engine/audio/core/AudioProcess.hpp>

#include <engine/audio/mixer/AudioMixer.hpp>
#include <engine/audio/mixer/AudioMixerBus.hpp>
#include <engine/audio/mixer/MainMixProcess.hpp>
#include <engine/audio/mixer/MixVariables.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;
using namespace std;

AudioMixerStrip::AudioMixerStrip(AudioMixer* mixer, shared_ptr<AudioControlsChain> controlsChain)
	: AudioProcessChain(controlsChain)
{
	silenceCountdown = silenceCount;
	this->mixer = mixer;
	buffer = createBuffer();
	channelFormat = buffer->getChannelFormat();
}

void AudioMixerStrip::setInputProcess(shared_ptr<AudioProcess> input)
{

	if (controlChain->getId() != MixerControlsIds::CHANNEL_STRIP) {
		return;
	}
	auto oldInput = this->input;
	if (input) input->open();

	this->input = input;
	if (oldInput)
		oldInput->close();
}

void AudioMixerStrip::setDirectOutputProcess(shared_ptr<AudioProcess> output)
{
	auto oldOutput = directOutput;
	if (output) output->open();

	this->directOutput = output;
	if (oldOutput) oldOutput->close();
}

void AudioMixerStrip::silence()
{
	if (nmixed > 0) {
		buffer->makeSilence();
		nmixed = 0;
	}
}

AudioBuffer* AudioMixerStrip::createBuffer()
{
	auto id = controlChain->getId();
	if (id == MixerControlsIds::CHANNEL_STRIP) {
		isChannel = true;
		return mixer->getSharedBuffer();
	}
	else if (id == MixerControlsIds::GROUP_STRIP) {
		auto buf = mixer->createBuffer(getName());
		buf->setChannelFormat(mixer->getMainBus()->getBuffer()->getChannelFormat());
		return buf;
	}
	else if (id == MixerControlsIds::MAIN_STRIP) {
		return mixer->getMainBus()->getBuffer();
	}
	return mixer->getBus(getName())->getBuffer();
}

const int AudioMixerStrip::silenceCount;

bool AudioMixerStrip::processBuffer(int nFrames)
{
	auto ret = AUDIO_OK;
	if (isChannel) {
		if (input) {
			ret = input->processAudio(buffer, nFrames);

			if (ret == AUDIO_DISCONNECT) {
				return false;
			}
			else if (ret == AUDIO_SILENCE && silenceCountdown == 0) {
				return false;
			}
		}
		else {
			return false;
		}
	}

	processAudio(buffer, nFrames);

	if (isChannel) {
		if (ret == AUDIO_SILENCE) {
			if (buffer->square() > 0.00000001f)
				silenceCountdown = silenceCount;
			else
				silenceCountdown--;
		}
		else {
			silenceCountdown = silenceCount;
		}
	}
	if (directOutput) {
		directOutput->processAudio(buffer, nFrames);
	}
	return true;
}

shared_ptr<AudioProcess> AudioMixerStrip::createProcess(shared_ptr<AudioControls> controls)
{
	auto mixVars = dynamic_pointer_cast<MixVariables>(controls);
	if (mixVars) {
		shared_ptr<AudioMixerStrip> routedStrip;
		if (mixVars->getName() == mixer->getMainBus()->getName()) {
			routedStrip = mixer->getMainStrip();
			try {
				return make_shared<MainMixProcess>(routedStrip, mixVars, mixer);
			}
			catch (bad_cast e) {
				printf("\n%s", e.what());
			}
		}
		else {
			routedStrip = mixer->getStripImpl(mixVars->getName());
			return make_shared<MixProcess>(routedStrip, mixVars);
		}
	}

	return AudioProcessChain::createProcess(controls);
}

int AudioMixerStrip::mix(mpc::engine::audio::core::AudioBuffer* bufferToMix, vector<float>& gain)
{
	if (bufferToMix == nullptr) return 0;
	auto ret = channelFormat->mix(buffer, bufferToMix, gain);
	if (ret != 0) nmixed += 1;
	return ret;
}

void AudioMixerStrip::close()
{
    AudioProcessChain::close();
    mixer->removeBuffer(buffer);
}

AudioMixerStrip::~AudioMixerStrip() {
}
