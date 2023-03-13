#include "MpcBasicSoundPlayerChannel.hpp"

#include "MpcBasicSoundPlayerControls.hpp"
#include "MpcVoice.hpp"
#include "MpcSampler.hpp"

#include <engine/audio/fader/FaderControl.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>

using namespace ctoot::mpc;
using namespace ctoot::audio::mixer;
using namespace ctoot::audio::fader;
using namespace std;

MpcBasicSoundPlayerChannel::MpcBasicSoundPlayerChannel(shared_ptr<MpcBasicSoundPlayerControls> controls)
{
	auto lControls = controls;
	sampler = lControls->getSampler();
	mixer = lControls->getMixer();
	voice = lControls->getVoice();
	auto sc = mixer->getMixerControls()->getStripControls("65");
	auto mmc = dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	fader = dynamic_pointer_cast<FaderControl>(mmc->find("Level"));
}

void MpcBasicSoundPlayerChannel::mpcNoteOn(int soundNumber, int velocity, int frameOffset)
{
	if (velocity == 0) {
		return;
	}

	tempVars.reset();

	if (soundNumber == -4) {
		tempVars = sampler->getPlayXSound();
	}
	else if (soundNumber == -3) {
		tempVars = sampler->getMpcPreviewSound();
	}
	else if (soundNumber == -2) {
		tempVars = sampler->getClickSound();
	}
	else if (soundNumber == -1) {
		tempVars = {};
	}
	else if (soundNumber >= 0) {
		tempVars = sampler->getMpcSound(soundNumber);
	}
    
	if (!tempVars) {
		return;
	}

	fader->setValue(soundNumber == -2 ? 200 : 100);
	voice->init(velocity, tempVars, -1, nullptr, 0, 64, -1, -1, frameOffset, soundNumber != -2, -1, -1);
}

void MpcBasicSoundPlayerChannel::finishVoice() {
	voice->finish(); // stops voice immediately, without a short fade-out/decay time
}

void MpcBasicSoundPlayerChannel::connectVoice()
{
    mixer->getStrip("65")->setInputProcess(voice);
}
