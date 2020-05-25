#include "ConvertSoundScreen.hpp"

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
		setConvert(i < 0 ? int(0) : 1);
	}
}

void ConvertSoundScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sound");
		break;
	case 4:
		if (convert == 0)
		{
			if (sampler.lock()->getSound().lock()->isMono())
			{
				ls.lock()->openScreen("monotostereo");
			}
			else
			{
				ls.lock()->openScreen("stereo-to-mono");
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

	if (convert == 0 && sampler.lock()->getSound().lock()->isMono())
	{
		findField("convert").lock()->setText("MONO TO STEREO");
	}
	else
	{
		findField("convert").lock()->setText(convertNames[convert]);
	}
}

void ConvertSoundScreen::setConvert(int i)
{
	convert = i;
	displayConvert();
}
