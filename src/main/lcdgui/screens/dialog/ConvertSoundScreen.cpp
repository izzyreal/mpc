#include "ConvertSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

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

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "convert")
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
        mpc.getLayeredScreen()->openScreen<SoundScreen>();
		break;
	case 4:
		if (convert == 0)
		{
			if (sampler->getSound()->isMono())
			{
        mpc.getLayeredScreen()->openScreen<MonoToStereoScreen>();
			}
			else
			{
        mpc.getLayeredScreen()->openScreen<StereoToMonoScreen>();
			}
		}
		else
		{
        mpc.getLayeredScreen()->openScreen<ResampleScreen>();
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
