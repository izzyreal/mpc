#include "MpcSoundPlayerControls.hpp"

#include "MpcSampler.hpp"
#include <engine/audio/mixer/AudioMixer.hpp>
#include <engine/audio/server/AudioServer.hpp>

using namespace ctoot::mpc;
using namespace std;

MpcSoundPlayerControls::MpcSoundPlayerControls(
	shared_ptr<MpcSampler> sampler,
    int drumNumber,
    shared_ptr<ctoot::audio::mixer::AudioMixer> mixer,
    shared_ptr<ctoot::audio::server::AudioServer> server,
    ctoot::mpc::MpcMixerSetupGui* mixerSetupGui,
    std::vector<std::shared_ptr<MpcVoice>> voicesToUse) :
    ctoot::control::CompoundControl(8, NAME_),
    voices(voicesToUse)
{
	this->sampler = sampler;
	this->drumNumber = drumNumber;
	this->mixer = mixer;
	this->server = server;
	this->mixerSetupGui = mixerSetupGui;
}

string MpcSoundPlayerControls::NAME_ = "MpcSoundPlayer";

std::vector<std::shared_ptr<MpcVoice>> MpcSoundPlayerControls::getVoices()
{
	return voices;
}

shared_ptr<ctoot::mpc::MpcSampler> MpcSoundPlayerControls::getSampler()
{
    return sampler;
}

int MpcSoundPlayerControls::getDrumIndex()
{
    return drumNumber;
}

shared_ptr<ctoot::audio::mixer::AudioMixer> MpcSoundPlayerControls::getMixer()
{
    return mixer;
}

ctoot::audio::server::AudioServer* MpcSoundPlayerControls::getServer()
{
    return server.get();
}

ctoot::mpc::MpcMixerSetupGui* MpcSoundPlayerControls::getMixerSetupGui() {
	return mixerSetupGui;
}
