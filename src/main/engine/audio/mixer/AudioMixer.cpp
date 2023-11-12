#include <engine/audio/mixer/AudioMixer.hpp>
#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include <engine/audio/mixer/AudioMixerBus.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixerControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/audio/server/AudioServer.hpp>

using namespace mpc::engine::audio::server;
using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;

AudioMixer::AudioMixer(const std::shared_ptr<MixerControls>& controlsToUse, const std::shared_ptr<AudioServer>& serverToUse)
: controls(controlsToUse), server(serverToUse)
{
	sharedAudioBuffer = server->createAudioBuffer("Mixer (shared)");
	createBusses(controls);
	createStrips(controls);
}

std::shared_ptr<MixerControls> AudioMixer::getMixerControls()
{
    return controls;
}

AudioBuffer* AudioMixer::getSharedBuffer()
{
    return sharedAudioBuffer;
}

AudioBuffer* AudioMixer::createBuffer(std::string name)
{
    return server->createAudioBuffer(name);
}

void AudioMixer::removeBuffer(AudioBuffer* buffer)
{
    server->removeAudioBuffer(buffer);
}

std::shared_ptr<AudioMixerStrip> AudioMixer::getStrip(std::string name)
{
    return getStripImpl(name);
}

std::shared_ptr<AudioMixerStrip> AudioMixer::getStripImpl(std::string name)
{
	for (auto& strip : strips) {
		if (strip->getName() == name) {
			return strip;
		}
	}
	return {};
}

void AudioMixer::work(int nFrames)
{
    AudioMixer::silenceStrips(auxStrips);
	mainStrip->silence();

    for (int i = 0; i < 32; i++)
    {
        auto voiceStrip = channelStrips[i];

        if (voiceStrip->processBuffer(nFrames))
        {
            auto indivOutStrip = channelStrips[i + 32];
            indivOutStrip->processBuffer(nFrames);
        }
    }

    for (int i = 64; i < channelStrips.size(); i++)
    {
        channelStrips[i]->processBuffer(nFrames);
    }

    AudioMixer::evaluateStrips(auxStrips, nFrames);
	mainStrip->processBuffer(nFrames);
	writeBusBuffers(nFrames);
}

void AudioMixer::evaluateStrips(std::vector<std::shared_ptr<AudioMixerStrip>>& stripsToEvaluate, int nFrames)
{
    for (auto& strip : stripsToEvaluate) {
		strip->processBuffer(nFrames);
	}
}

void AudioMixer::silenceStrips(std::vector<std::shared_ptr<AudioMixerStrip>>& stripsToSilence)
{
	for (auto& strip : stripsToSilence)
		strip->silence();
}

void AudioMixer::writeBusBuffers(int nFrames)
{
	for (auto& bus : busses)
		bus->write(nFrames);
}

void AudioMixer::createBusses(std::shared_ptr<MixerControls> mixerControls)
{
	busses.clear();
	auxBusses.clear();

	for (auto& bc : mixerControls->getAuxBusControls()) {
		auto bus = createBus(bc);
		busses.push_back(bus);
		auxBusses.push_back(bus);
	}

	mainBus = createBus(mixerControls->getMainBusControls());
	busses.push_back(mainBus);
}

std::shared_ptr<AudioMixerBus> AudioMixer::createBus(std::shared_ptr<BusControls> busControls)
{
	return std::make_shared<AudioMixerBus>(this, busControls);
}

std::shared_ptr<AudioMixerBus> AudioMixer::getBus(std::string name)
{
	for (auto& bus : busses) {
		if (bus->getName() == name) {
			return bus;
		}
	}
	return nullptr;
}

std::shared_ptr<AudioMixerBus> AudioMixer::getMainBus()
{
    return mainBus;
}

std::shared_ptr<AudioMixerStrip> AudioMixer::getMainStrip()
{
	if (!mainStrip) {
		return {};
	}
	return mainStrip;
}

void AudioMixer::createStrips(std::shared_ptr<MixerControls> mixerControls)
{
	for (auto& control : mixerControls->getControls()) {
		auto candidate = std::dynamic_pointer_cast<core::AudioControlsChain>(control);
		if (candidate) {
			createStrip(candidate);
		}
	}
}

std::shared_ptr<AudioMixerStrip> AudioMixer::createStrip(std::shared_ptr<AudioControlsChain> controls)
{

	auto strip = std::make_shared<AudioMixerStrip>(this, controls);

	switch (controls->getId()) {
	case MixerControlsIds::CHANNEL_STRIP:
		channelStrips.push_back(strip);
		break;
	case MixerControlsIds::AUX_STRIP:
		auxStrips.push_back(strip);
		break;
	case MixerControlsIds::MAIN_STRIP:
		if (!mainStrip) {
			mainStrip = strip;
		}
		break;
	}

	try {
		strips.push_back(strip);
		strip->open();
	} catch (const std::exception&) {
	}

    return strip;
}

void AudioMixer::close()
{
	for (auto& s : strips) {
		s->close();
	}
	strips.clear();

	channelStrips.clear();
	auxStrips.clear();

    busses.clear();
	auxBusses.clear();
	server->removeAudioBuffer(sharedAudioBuffer);
	server.reset();
	controls.reset();
}
