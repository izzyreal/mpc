#include <controls/sampler/dialog/CopySoundControls.hpp>

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

CopySoundControls::CopySoundControls() 
	: ScreenComponent("", layerIndex)
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
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(sampler.lock()->getSoundIndex()).lock());
		newSound = sampler.lock()->copySound(sound);
		newSound.lock()->setName(soundGui->getNewName());
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		lLs->openScreen("sound");
		break;
	}
}

void CopySoundControls::turnWheel(int i)
{
	init();
	
	if (param.compare("snd") == 0)
	{
		sampler.lock()->setSoundIndex(sampler.lock()->getNextSoundIndex(sampler.lock()->getSoundIndex(), i > 0));
		auto newSampleName = sampler.lock()->getSoundName(sampler.lock()->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		soundGui->setNewName(newSampleName);
	}

	if (param.compare("newname") == 0)
	{
		nameGui->setName(ls.lock()->lookupField("newname").lock()->getText());
		nameGui->setParameterName("newname");
		ls.lock()->openScreen("name");
	}
}
