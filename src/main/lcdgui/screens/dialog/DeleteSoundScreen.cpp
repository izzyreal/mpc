#include "DeleteSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteSoundScreen::DeleteSoundScreen(mpc::Mpc& mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-sound", layerIndex)
{
}

void DeleteSoundScreen::open()
{
    displaySnd();
}

void DeleteSoundScreen::function(int i)
{
    init();

    switch (i)
    {
        case 2:
            mpc.getLayeredScreen()->openScreen<DeleteAllSoundScreen>();
            break;
        case 3:
            mpc.getLayeredScreen()->openScreen<SoundScreen>();
            break;
        case 4:
            sampler->deleteSound(sampler->getSoundIndex());

            if (sampler->getSoundIndex() > sampler->getSoundCount() - 1)
                sampler->setSoundIndex(sampler->getSoundCount() - 1);

            if (sampler->getSoundCount() > 0)
                mpc.getLayeredScreen()->openScreen<SoundScreen>();
            else
                mpc.getLayeredScreen()->openScreen(sampler->getPreviousScreenName());

            break;
    }
}

void DeleteSoundScreen::turnWheel(int i)
{
    init();

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
