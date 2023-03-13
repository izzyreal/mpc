#include <engine/audio/mixer/AudioMixer.hpp>
#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include <engine/audio/mixer/AudioMixerBus.hpp>
#include <engine/audio/mixer/AudioMixerStrip.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixerControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/audio/server/AudioServer.hpp>

using namespace std;
using namespace ctoot::audio::server;
using namespace ctoot::audio::mixer;

AudioMixer::AudioMixer(shared_ptr<MixerControls> controls, shared_ptr<AudioServer> server)
{
	this->controls = controls;
	this->server = server;
	sharedAudioBuffer = server->createAudioBuffer("Mixer (shared)");
	createBusses(controls);
	createStrips(controls);
}

std::shared_ptr<ctoot::audio::server::AudioServer> AudioMixer::getAudioServer() {
	return server;
}

shared_ptr<MixerControls> AudioMixer::getMixerControls()
{
    return controls;
}

ctoot::audio::core::AudioBuffer* AudioMixer::getSharedBuffer()
{
    return sharedAudioBuffer;
}

ctoot::audio::core::AudioBuffer* AudioMixer::createBuffer(string name)
{
    return server->createAudioBuffer(name);
}

void AudioMixer::removeBuffer(ctoot::audio::core::AudioBuffer* buffer)
{
    server->removeAudioBuffer(buffer);
}

shared_ptr<AudioMixerStrip> AudioMixer::getStrip(string name)
{
    return getStripImpl(name);
}

shared_ptr<AudioMixerStrip> AudioMixer::getStripImpl(string name)
{
	for (auto& strip : strips) {
		if (strip->getName().compare(name) == 0) {
			return strip;
		}
	}
	return {};
}

void AudioMixer::work(int nFrames)
{
	silenceStrips(&groupStrips);
	silenceStrips(&auxStrips);
	mainStrip->silence();
	evaluateStrips(&channelStrips, nFrames);
	evaluateStrips(&groupStrips, nFrames);
	evaluateStrips(&auxStrips, nFrames);
	mainStrip->processBuffer(nFrames);
	writeBusBuffers(nFrames);
}

void AudioMixer::evaluateStrips(vector<shared_ptr<AudioMixerStrip>>* strips, int nFrames)
{
	for (auto& strip : (*strips)) {
		strip->processBuffer(nFrames);
	}
}

void AudioMixer::silenceStrips(vector<shared_ptr<AudioMixerStrip>>* strips)
{
	for (auto& strip : (*strips))
		strip->silence();
}

void AudioMixer::writeBusBuffers(int nFrames)
{
	for (auto& bus : busses)
		bus->write(nFrames);
}

void AudioMixer::createBusses(shared_ptr<MixerControls> mixerControls)
{
	busses.clear();
	auxBusses.clear();

	shared_ptr<AudioMixerBus> bus;

	for (auto& bc : mixerControls->getAuxBusControls()) {
		bus = createBus(bc);
		busses.push_back(bus);
		auxBusses.push_back(bus);
	}

	mainBus = createBus(mixerControls->getMainBusControls());
	busses.push_back(mainBus);
}

shared_ptr<AudioMixerBus> AudioMixer::createBus(shared_ptr<BusControls> busControls)
{
	return make_shared<AudioMixerBus>(this, busControls);
}

shared_ptr<AudioMixerBus> AudioMixer::getBus(string name)
{
	for (auto& bus : busses) {
		if (bus->getName().compare(name) == 0) {
			return bus;
		}
	}
	return nullptr;
}

shared_ptr<AudioMixerBus> AudioMixer::getMainBus()
{
    return mainBus;
}

shared_ptr<AudioMixerStrip> AudioMixer::getMainStrip()
{
	if (!mainStrip) {
		return {};
	}
	return mainStrip;
}

void AudioMixer::createStrips(shared_ptr<MixerControls> mixerControls)
{
	for (auto& control : mixerControls->getControls()) {
		auto candidate = dynamic_pointer_cast<core::AudioControlsChain>(control);
		if (candidate) {
			createStrip(candidate);
		}
	}
}

shared_ptr<AudioMixerStrip> AudioMixer::createStrip(shared_ptr<ctoot::audio::core::AudioControlsChain> controls)
{

	auto strip = make_shared<AudioMixerStrip>(this, controls);

	switch (controls->getId()) {
	case MixerControlsIds::CHANNEL_STRIP:
		channelStrips.push_back(strip);
		break;
	case MixerControlsIds::GROUP_STRIP:
		groupStrips.push_back(strip);
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
	groupStrips.clear();
	auxStrips.clear();

	for (auto& b : busses) {
		b->close();
	}
    busses.clear();
	auxBusses.clear();
	server->removeAudioBuffer(sharedAudioBuffer);
	server.reset();
	controls.reset();
}
