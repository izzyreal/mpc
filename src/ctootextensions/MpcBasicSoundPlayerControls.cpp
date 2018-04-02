#include <ctootextensions/MpcBasicSoundPlayerControls.hpp>

#include <ctootextensions/MpcEnvelopeControls.hpp>
#include <ctootextensions/MpcSoundOscillatorControls.hpp>
#include <control/ControlRow.hpp>
#include <control/Control.hpp>
//#include <synth/modules/amplifier/AmplifierControls.hpp>

using namespace mpc::ctootextensions;
using namespace std;

MpcBasicSoundPlayerControls::MpcBasicSoundPlayerControls(weak_ptr<mpc::sampler::Sampler> sampler, shared_ptr<ctoot::audio::mixer::AudioMixer> mixer, weak_ptr<mpc::ctootextensions::Voice> voice) : SynthChannelControls(MPC_BASIC_SOUND_PLAYER_CHANNEL_ID, "MpcBasicSoundPlayerChannel")
{
	this->sampler = sampler;
	this->mixer = mixer;
	this->voice = voice;
}

const int MpcBasicSoundPlayerControls::MPC_BASIC_SOUND_PLAYER_CHANNEL_ID;

string& MpcBasicSoundPlayerControls::NAME()
{
    return NAME_;
}
string MpcBasicSoundPlayerControls::NAME_ = "MpcBasicSoundPlayer";

weak_ptr<mpc::sampler::Sampler> MpcBasicSoundPlayerControls::getSampler()
{
    return sampler;
}

weak_ptr<ctoot::audio::mixer::AudioMixer> MpcBasicSoundPlayerControls::getMixer()
{
    return mixer;
}

weak_ptr<mpc::ctootextensions::Voice> MpcBasicSoundPlayerControls::getVoice()
{
    return voice;
}

MpcBasicSoundPlayerControls::~MpcBasicSoundPlayerControls() {
}
