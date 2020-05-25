#include "ConvertSoundScreen.hpp"

#include <ui/sampler/SoundGui.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

ConvertSoundScreen::ConvertSoundScreen(const int layerIndex) 
	: ScreenComponent("convertsound", layerIndex)
{
}

void ConvertSoundScreen::open()
{
	displayConvert();
}

void ConvertSoundScreen::turnWheel(int i)
{
	init();

	if (param.compare("convert") == 0)
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		soundGui->setConvert(i < 0 ? int(0) : 1);
		displayConvert();
	}
}

void ConvertSoundScreen::function(int i)
{
	init();
	
	auto soundGui = mpc.getUis().lock()->getSoundGui();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sound");
		break;
	case 4:
		if (soundGui->getConvert() == 0)
		{
			string name = sampler.lock()->getSound().lock()->getName();
			name = StrUtil::trim(name);
			name = StrUtil::padRight(name, "_", 16);
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
		else
		{
			ls.lock()->openScreen("resample");
		}
	}
}

void ConvertSoundScreen::displayConvert()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();

	if (soundGui->getConvert() == 0 && sampler.lock()->getSound().lock()->isMono())
	{
		findField("convert").lock()->setText("MONO TO STEREO");
	}
	else
	{
		findField("convert").lock()->setText(convertNames[soundGui->getConvert()]);
	}
}
