#include "DeleteSoundScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteSoundScreen::DeleteSoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-sound", layerIndex)
{
}

void DeleteSoundScreen::open()
{
    displaySnd();
}

void DeleteSoundScreen::function(int i)
{
    switch (i)
    {
        case 2:
            openScreenById(ScreenId::DeleteAllSoundScreen);
            break;
        case 3:
            openScreenById(ScreenId::SoundScreen);
            break;
        case 4:
            sampler->deleteSound(sampler->getSoundIndex());

            if (sampler->getSoundIndex() > sampler->getSoundCount() - 1)
            {
                sampler->setSoundIndex(sampler->getSoundCount() - 1);
            }

            if (sampler->getSoundCount() > 0)
            {
                openScreenById(ScreenId::SoundScreen);
            }
            else
            {
                ls->openScreen(sampler->getPreviousScreenName());
            }

            break;
    }
}

void DeleteSoundScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "snd")
    {
        sampler->setSoundIndex(sampler->getSoundIndex() + i);
        displaySnd();
    }
}

void DeleteSoundScreen::displaySnd()
{
    if (!sampler->getSound())
    {
        return;
    }

    findField("snd")->setText(sampler->getSound()->getName());
}
