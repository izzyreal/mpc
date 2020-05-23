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
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sound");
		break;
	case 4:
		if (soundGui->getConvert() == 0)
		{
			string name = sampler.lock()->getSound().lock()->getName();
			name = moduru::lang::StrUtil::trim(name);
			name = moduru::lang::StrUtil::padRight(name, "_", 16);
			name = name.substr(0, 14);

			if (sampler.lock()->getSound().lock()->isMono())
			{
				soundGui->setNewStName(name + "-S");
				soundGui->setRSource(sampler.lock()->getSoundIndex(), sampler.lock()->getSoundCount());
				ls.lock()->openScreen("monotostereo");
			}
			else
			{
				soundGui->setNewLName(name + "-L");
				soundGui->setNewRName(name + "-R");
				ls.lock()->openScreen("stereotomono");
			}
		}
		else {
			ls.lock()->openScreen("resample");
			soundGui->setNewFs(sampler.lock()->getSound().lock()->getSampleRate());
			auto newSampleName = sampler.lock()->getSound().lock()->getName();
			//newSampleName = newSampleName->replaceAll("\\s+$", "");
			newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
			soundGui->setNewName(newSampleName);
		}
	}
}
