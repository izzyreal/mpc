#include <engine/mpc/MpcBasicSoundPlayerControls.hpp>

#include <engine/control/Control.hpp>

using namespace ctoot::mpc;
using namespace std;

MpcBasicSoundPlayerControls::MpcBasicSoundPlayerControls(shared_ptr<ctoot::mpc::MpcSampler> sampler, shared_ptr<ctoot::audio::mixer::AudioMixer> mixer, shared_ptr<ctoot::mpc::MpcVoice> voice)
	: ctoot::control::CompoundControl(9, "MpcBasicSoundPlayerChannel")
{
	this->sampler = sampler;
	this->mixer = mixer;
	this->voice = voice;
}

string& MpcBasicSoundPlayerControls::NAME()
{
    return NAME_;
}
string MpcBasicSoundPlayerControls::NAME_ = "MpcBasicSoundPlayer";

shared_ptr<ctoot::mpc::MpcSampler> MpcBasicSoundPlayerControls::getSampler()
{
    return sampler;
}

shared_ptr<ctoot::audio::mixer::AudioMixer> MpcBasicSoundPlayerControls::getMixer()
{
    return mixer;
}

shared_ptr<ctoot::mpc::MpcVoice> MpcBasicSoundPlayerControls::getVoice()
{
    return voice;
}
