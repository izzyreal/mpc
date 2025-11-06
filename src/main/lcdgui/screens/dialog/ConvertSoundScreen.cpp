#include "ConvertSoundScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;

ConvertSoundScreen::ConvertSoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "convert-sound", layerIndex)
{
}

void ConvertSoundScreen::open()
{
    displayConvert();
}

void ConvertSoundScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "convert")
    {
        setConvert(i < 0 ? 0 : 1);
    }
}

void ConvertSoundScreen::function(const int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SoundScreen);
            break;
        case 4:
            if (convert == 0)
            {
                if (sampler->getSound()->isMono())
                {
                    openScreenById(ScreenId::MonoToStereoScreen);
                }
                else
                {
                    openScreenById(ScreenId::StereoToMonoScreen);
                }
            }
            else
            {
                openScreenById(ScreenId::ResampleScreen);
            }
    }
}

void ConvertSoundScreen::displayConvert() const
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

void ConvertSoundScreen::setConvert(const int i)
{
    convert = i;
    displayConvert();
}
