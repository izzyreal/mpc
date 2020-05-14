#include <controls/sampler/dialog/ConvertSoundControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lcdgui/Wave.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

ConvertSoundControls::ConvertSoundControls() 
	: AbstractSamplerControls()
{
}

void ConvertSoundControls::turnWheel(int i)
{
	init();
	if (param.compare("convert") == 0) {
		soundGui->setConvert(i < 0 ? int(0) : 1);
	}
}

void ConvertSoundControls::function(int i)
{
	init();
	
	auto lLs = ls.lock();
	switch (i) {
	case 3:
		lLs->openScreen("sound");
		break;
	case 4:
		if (soundGui->getConvert() == 0) {
			string name = sampler.lock()->getSoundName(soundGui->getSoundIndex());
			name = moduru::lang::StrUtil::trim(name);
			name = moduru::lang::StrUtil::padRight(name, "_", 16);
			name = name.substr(0, 14);
			if (sampler.lock()->getSound(soundGui->getSoundIndex()).lock()->isMono()) {
				soundGui->setNewStName(name + "-S");
				soundGui->setRSource(soundGui->getSoundIndex(), sampler.lock()->getSoundCount());
				lLs->openScreen("monotostereo");
			}
			else {
				soundGui->setNewLName(name + "-L");
				soundGui->setNewRName(name + "-R");
				lLs->openScreen("stereotomono");
			}
		}
		else {
			lLs->openScreen("resample");
			soundGui->setNewFs(dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getSoundIndex()).lock())->getSampleRate());
			auto newSampleName = sampler.lock()->getSoundName(soundGui->getSoundIndex());
			//newSampleName = newSampleName->replaceAll("\\s+$", "");
			newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
			soundGui->setNewName(newSampleName);
		}
	}
}
