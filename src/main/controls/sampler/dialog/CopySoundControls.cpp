#include <controls/sampler/dialog/CopySoundControls.hpp>

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

CopySoundControls::CopySoundControls() 
	: AbstractSamplerControls()
{
}

void CopySoundControls::function(int i)
{
	init();
	auto lLs = ls.lock();
	weak_ptr<mpc::sampler::Sound> sound;
	weak_ptr<mpc::sampler::Sound> newSound;
	auto lSampler = sampler.lock();
	switch (i) {
	case int(3) :
		lLs->openScreen("sound");
		break;
	case 4:
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
		newSound = lSampler->copySound(sound);
		newSound.lock()->setName(soundGui->getNewName());
		soundGui->setSoundIndex(lSampler->getSoundCount() - 1, lSampler->getSoundCount());
		lLs->openScreen("sound");
		break;
	}
}

void CopySoundControls::turnWheel(int i)
{
	init();
	auto lSampler = sampler.lock();
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(lSampler->getNextSoundIndex(soundGui->getSoundIndex(), i > 0), lSampler->getSoundCount());
		auto newSampleName = lSampler->getSoundName(soundGui->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = lSampler->addOrIncreaseNumber(newSampleName);
		soundGui->setNewName(newSampleName);
	}

	auto lLs = ls.lock();
	if (param.compare("newname") == 0) {
		nameGui->setName(lLs->lookupField("newname").lock()->getText());
		nameGui->setParameterName("newname");
		lLs->openScreen("name");
	}
}
