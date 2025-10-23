#include "PreviewSoundPlayer.hpp"

#include "Voice.hpp"

#include <sampler/Sampler.hpp>
#include "FaderControl.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"
#include <utility>

using namespace mpc::sampler;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;

PreviewSoundPlayer::PreviewSoundPlayer(
        std::shared_ptr<Sampler> samplerToUse,
        std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixerToUse,
        std::shared_ptr<Voice> voiceToUse)
        : sampler(std::move(samplerToUse)), mixer(std::move(mixerToUse)), voice(std::move(voiceToUse))
{
	auto sc = mixer->getMixerControls()->getStripControls("65");
	auto mmc =std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	fader = std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"));
}

void PreviewSoundPlayer::mpcNoteOn(int soundNumber, int velocity, int frameOffset)
{
	if (velocity == 0)
    {
		return;
	}

	tempVars.reset();

	if (soundNumber == PLAYX_SOUND)
    {
		tempVars = sampler->getPlayXSound();
	}
	else if (soundNumber == PREVIEW_SOUND)
    {
		tempVars = sampler->getPreviewSound();
	}
	else if (soundNumber == CLICK_SOUND)
    {
		tempVars = sampler->getClickSound();
	}
	else if (soundNumber == NO_SOUND)
    {
		tempVars = {};
	}
	else if (soundNumber >= 0)
    {
		tempVars = sampler->getSound(soundNumber);
	}
    
	if (!tempVars)
    {
		return;
	}

    soundHasLoop = tempVars->isLoopEnabled();

	fader->setValue(soundNumber == -2 ? 200 : 100);
	voice->init(velocity, tempVars, -1, nullptr, 0, 64, frameOffset, soundNumber != -2, -1, -1, mixer->getSharedBuffer()->getSampleRate(), 1);
}

void PreviewSoundPlayer::finishVoice() {
	voice->finish(); // stops voice immediately, without a short fade-out/decay time
}

void PreviewSoundPlayer::finishVoiceIfSoundIsLooping()
{
    if (!soundHasLoop)
    {
        return;
    }

	voice->startDecay();
}

void PreviewSoundPlayer::connectVoice()
{
    mixer->getStrip("65")->setInputProcess(voice);
}
