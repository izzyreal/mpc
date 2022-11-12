#include "ConvertSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;

ConvertSoundScreen::ConvertSoundScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "convert-sound", layerIndex)
{
}

void ConvertSoundScreen::open()
{
	displayConvert();
}

void ConvertSoundScreen::turnWheel(int i)
{
	init();

	if (param == "convert")
	{
		setConvert(i < 0 ? 0 : 1);
	}
}

void ConvertSoundScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("sound");
		break;
	case 4:
		if (convert == 0)
		{
			if (sampler->getSound()->isMono())
			{
				openScreen("mono-to-stereo");
			}
			else
			{
				openScreen("stereo-to-mono");
			}
		}
		else
		{
			openScreen("resample");
		}
	}
}

void ConvertSoundScreen::displayConvert()
{
	if (!sampler->getSound())
	{
		return;
	}

	if (convert == 0 && sampler->getSound()->isMono())
	{
		findField("convert")->setText("MONO TO STEREO");
	}
	else
	{
		findField("convert")->setText(convertNames[convert]);
	}
}

void ConvertSoundScreen::setConvert(int i)
{
	convert = i;
	displayConvert();
}
