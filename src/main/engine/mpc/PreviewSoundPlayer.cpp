#include "PreviewSoundPlayer.hpp"

#include "MpcVoice.hpp"
#include "MpcSampler.hpp"

#include <engine/mpc/MpcFaderControl.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>
#include <utility>

using namespace ctoot::mpc;
using namespace ctoot::audio::mixer;

PreviewSoundPlayer::PreviewSoundPlayer(
        std::shared_ptr<MpcSampler> samplerToUse,
        std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixerToUse,
        std::shared_ptr<MpcVoice> voiceToUse)
        : sampler(std::move(samplerToUse)), mixer(std::move(mixerToUse)), voice(std::move(voiceToUse))
{
	auto sc = mixer->getMixerControls()->getStripControls("65");
	auto mmc =std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	fader = std::dynamic_pointer_cast<MpcFaderControl>(mmc->find("Level"));
}

void PreviewSoundPlayer::mpcNoteOn(int soundNumber, int velocity, int frameOffset)
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

void PreviewSoundPlayer::finishVoice() {
	voice->finish(); // stops voice immediately, without a short fade-out/decay time
}

void PreviewSoundPlayer::connectVoice()
{
    mixer->getStrip("65")->setInputProcess(voice);
}
