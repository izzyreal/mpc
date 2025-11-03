#include "CopySoundScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

CopySoundScreen::CopySoundScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-sound", layerIndex)
{
}

void CopySoundScreen::open()
{
    if (ls->isPreviousScreenNot<ScreenId::NameScreen>() && sampler->getSound())
    {
        newName = sampler->getSound()->getName();
        newName = sampler->addOrIncreaseNumber(newName);
    }
    displaySnd();
    displayNewName();
}

void CopySoundScreen::function(int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SoundScreen);
            break;
        case 4:
        {
            auto sound = sampler->getSound();
            auto newSound = sampler->copySound(sound);

            if (newSound.lock() == nullptr)
            {
                return;
            }

            newSound.lock()->setName(newName);
            sampler->setSoundIndex(sampler->getSoundCount() - 1);
            openScreenById(ScreenId::SoundScreen);
            break;
        }
    }
}

void CopySoundScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "snd")
    {
        sampler->nudgeSoundIndex(i > 0);
        auto newSampleName = sampler->getSoundName(sampler->getSoundIndex());
        newSampleName = sampler->addOrIncreaseNumber(newSampleName);
        setNewName(newSampleName);
        displaySnd();
    }

    openNameScreen();
}

void CopySoundScreen::openNameScreen()
{
    const auto enterAction = [this](std::string &nameScreenName)
    {
        if (sampler->isSoundNameOccupied(nameScreenName))
        {
            return;
        }

        newName = nameScreenName;
        openScreenById(ScreenId::CopySoundScreen);
    };

    const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    nameScreen->initialize(newName, 16, enterAction, "copy-sound");
    openScreenById(ScreenId::NameScreen);
}

void CopySoundScreen::displayNewName()
{
    findField("newname")->setText(newName);
}

void CopySoundScreen::displaySnd()
{
    if (!sampler->getSound())
    {
        return;
    }

    findField("snd")->setText(sampler->getSound()->getName());
}

void CopySoundScreen::setNewName(std::string s)
{
    newName = s;
    displayNewName();
}
