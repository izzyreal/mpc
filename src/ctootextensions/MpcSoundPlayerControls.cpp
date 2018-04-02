#include <ctootextensions/MpcSoundPlayerControls.hpp>

#include <sampler/Sampler.hpp>
#include <audio/mixer/AudioMixer.hpp>
#include <audio/server/AudioServer.hpp>

using namespace mpc::ctootextensions;
using namespace std;

MpcSoundPlayerControls::MpcSoundPlayerControls(weak_ptr<MpcMultiMidiSynth> mms,
	weak_ptr<mpc::sampler::Sampler> sampler
	, int drumNumber
	, weak_ptr<ctoot::audio::mixer::AudioMixer> mixer
	, weak_ptr<ctoot::audio::server::AudioServer> server
	, mpc::ui::sampler::MixerSetupGui* mixerSetupGui)
	: ctoot::synth::SynthChannelControls(MPC_SOUND_PLAYER_CHANNEL_ID, NAME_)
{
	this->mms = mms;
	this->sampler = sampler;
	this->drumNumber = drumNumber;
	this->mixer = mixer;
	this->server = server;
	this->mixerSetupGui = mixerSetupGui;
}

const int MpcSoundPlayerControls::MPC_SOUND_PLAYER_CHANNEL_ID;

string MpcSoundPlayerControls::NAME()
{
    return NAME_;
}
string MpcSoundPlayerControls::NAME_ = "MpcSoundPlayer";

weak_ptr<MpcMultiMidiSynth> MpcSoundPlayerControls::getMms() {
	return mms;
}

weak_ptr<mpc::sampler::Sampler> MpcSoundPlayerControls::getSampler()
{
    return sampler;
}

int MpcSoundPlayerControls::getDrumNumber()
{
    return drumNumber;
}

weak_ptr<ctoot::audio::mixer::AudioMixer> MpcSoundPlayerControls::getMixer()
{
    return mixer;
}

ctoot::audio::server::AudioServer* MpcSoundPlayerControls::getServer()
{
    return server.lock().get();
}

mpc::ui::sampler::MixerSetupGui* MpcSoundPlayerControls::getMixerSetupGui() {
	return mixerSetupGui;
}

MpcSoundPlayerControls::~MpcSoundPlayerControls() {
}
