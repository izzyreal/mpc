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
	
	switch (i) {
	case int(3) :
		lLs->openScreen("sound");
		break;
	case 4:
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getSoundIndex()).lock());
		newSound = sampler.lock()->copySound(sound);
		newSound.lock()->setName(soundGui->getNewName());
		soundGui->setSoundIndex(sampler.lock()->getSoundCount() - 1, sampler.lock()->getSoundCount());
		lLs->openScreen("sound");
		break;
	}
}

void CopySoundControls::turnWheel(int i)
{
	init();
	
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(sampler.lock()->getNextSoundIndex(soundGui->getSoundIndex(), i > 0), sampler.lock()->getSoundCount());
		auto newSampleName = sampler.lock()->getSoundName(soundGui->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		soundGui->setNewName(newSampleName);
	}

	auto lLs = ls.lock();
	if (param.compare("newname") == 0) {
		nameGui->setName(lLs->lookupField("newname").lock()->getText());
		nameGui->setParameterName("newname");
		lLs->openScreen("name");
	}
}
