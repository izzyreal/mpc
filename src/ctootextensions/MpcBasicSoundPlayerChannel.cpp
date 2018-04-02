#include <ctootextensions/MpcBasicSoundPlayerChannel.hpp>
#include <ctootextensions/MpcBasicSoundPlayerControls.hpp>
#include <ctootextensions/Voice.hpp>
#include <audio/core/AudioControlsChain.hpp>
#include <audio/fader/FaderControl.hpp>
#include <audio/mixer/AudioMixer.hpp>
#include <audio/mixer/AudioMixerStrip.hpp>
#include <audio/mixer/MainMixControls.hpp>
#include <audio/mixer/MixerControls.hpp>
#include <control/Control.hpp>
#include <ctootextensions/MpcBasicSoundPlayerControls.hpp>

#include <audio/mixer/MixerControlsFactory.hpp>

#include <sampler/Sampler.hpp>

#include <Mpc.hpp>

using namespace mpc::ctootextensions;
using namespace std;

MpcBasicSoundPlayerChannel::MpcBasicSoundPlayerChannel(weak_ptr<MpcBasicSoundPlayerControls> controls)
{
	auto lControls = controls.lock();
	sampler = lControls->getSampler();
	mixer = lControls->getMixer();
	voice = lControls->getVoice();
	auto lMixer = mixer.lock();
	mixerStrip = lMixer->getStrip("65");
	auto sc = lMixer->getMixerControls().lock()->getStripControls("65").lock();
	auto mmc = dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main").lock());
	fader = dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock());
}

void MpcBasicSoundPlayerChannel::setLocation(string location)
{

}

void MpcBasicSoundPlayerChannel::noteOn(int soundNumber, int velocity)
{
	mpcNoteOn(soundNumber, velocity, 0);
}

void MpcBasicSoundPlayerChannel::mpcNoteOn(int soundNumber, int velocity, int frameOffset)
{
	if (velocity == 0) {
		return;
	}

	tempVars.reset();
	auto lSampler = sampler.lock();
	if (soundNumber == -4) {
		tempVars = lSampler->getPlayXSound();
	}
	else if (soundNumber == -3) {
		tempVars = lSampler->getPreviewSound();
	}
	else if (soundNumber == -2) {
		tempVars = lSampler->getClickSound();
	}
	else if (soundNumber == -1) {
		tempVars = weak_ptr<mpc::sampler::Sound>();
	}
	else if (soundNumber >= 0) {
		tempVars = lSampler->getSound(soundNumber);
	}
	if (!tempVars.lock()) {
		return;
	}

	fader.lock()->setValue(soundNumber == -2 ? 200 : 100);
	voice.lock()->init(-1, velocity, -1, tempVars, nullptr, 0, 64, -1, -1, frameOffset, soundNumber == -2 ? false : true);
}

void MpcBasicSoundPlayerChannel::noteOff(int note)
{
}

void MpcBasicSoundPlayerChannel::allNotesOff()
{
}

void MpcBasicSoundPlayerChannel::allSoundOff()
{
	voice.lock()->startDecay();
}

void MpcBasicSoundPlayerChannel::finishVoice() {
	voice.lock()->finish(); // stops voice immediately, without a short fade-out/decay time
}

void MpcBasicSoundPlayerChannel::connectVoice()
{
	auto lMixer = mixer.lock();
	lMixer->getStrip("65").lock()->setInputProcess(voice);
}

void MpcBasicSoundPlayerChannel::noteOff(int pitch, int velocity)
{
	super::noteOff(pitch, velocity);
}

MpcBasicSoundPlayerChannel::~MpcBasicSoundPlayerChannel() {
}
