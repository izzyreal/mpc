#include <controls/sampler/SndParamsControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler;
using namespace std;

SndParamsControls::SndParamsControls() 
	: AbstractSamplerControls()
{
}

void SndParamsControls::openWindow()
{
	init();
	auto lLs = ls.lock();
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(soundGui->getSoundIndex(), sampler.lock()->getSoundCount());
		soundGui->setPreviousScreenName("params");
		lLs->openScreen("sound");
	}
}

void SndParamsControls::function(int f)
{
	init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	string newSampleName;
	vector<int> zone;
	switch (f) {
	case 0:
		lLs->openScreen("trim");
		break;
	case 1:
		lLs->openScreen("loop");
		break;
	case 2:
		lLs->openScreen("zone");
		break;
	case 3:
		lSampler->sort();
		break;
	case 4:
		if (lSampler->getSoundCount() == 0)
			return;

		newSampleName = lSampler->getSoundName(soundGui->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = lSampler->addOrIncreaseNumber(newSampleName);
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("trim");
		lLs->openScreen("editsound");
		break;
	case 5:
		if (Mpc::instance().getControls().lock()->isF6Pressed()) {
			return;
		}
		
		Mpc::instance().getControls().lock()->setF6Pressed(true);
		
		zone = vector<int>{ soundGui->getZoneStart(soundGui->getZoneNumber()) , soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		lSampler->playX(soundGui->getPlayX(), &zone);
		break;
	}
}

void SndParamsControls::turnWheel(int i)
{
	init();
	auto lSampler = sampler.lock();
	auto lSound = sound.lock();
	if (param.compare("playx") == 0) {
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("snd") == 0 && i > 0) {
		lSampler->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0) {
		lSampler->setSoundGuiPrevSound();
	}
	else if (param.compare("level") == 0) {
		lSound->setLevel(lSound->getSndLevel() + i);
	}
	else if (param.compare("tune") == 0) {
		lSound->setTune(lSound->getTune() + i);
	}
	else if (param.compare("beat") == 0) {
		lSound->setNumberOfBeats(lSound->getBeatCount() + i);
	}
}
