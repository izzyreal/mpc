#include "CopySoundScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

CopySoundScreen::CopySoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-sound", layerIndex)
{
}

void CopySoundScreen::open()
{
    if (ls.lock()->isPreviousScreenNot({ScreenId::NameScreen}) &&
        sampler.lock()->getSound())
    {
        newName = sampler.lock()->getSound()->getName();
        newName = sampler.lock()->addOrIncreaseNumber(newName);
    }
    displaySnd();
    displayNewName();
}

void CopySoundScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SoundScreen);
            break;
        case 4:
        {
            const auto sound = sampler.lock()->getSound();
            const auto newSound = sampler.lock()->copySound(sound);

            if (newSound.lock() == nullptr)
            {
                return;
            }

            newSound.lock()->setName(newName);
            sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
            openScreenById(ScreenId::SoundScreen);
            break;
        }
        default:;
    }
}

void CopySoundScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "snd")
    {
        sampler.lock()->nudgeSoundIndex(i > 0);
        auto newSampleName =
            sampler.lock()->getSoundName(sampler.lock()->getSoundIndex());
        newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
        setNewName(newSampleName);
        displaySnd();
    }

    openNameScreen();
}

void CopySoundScreen::openNameScreen()
{
    if (getFocusedFieldNameOrThrow() != "newname")
    {
        return;
    }

    const auto enterAction = [this](const std::string &nameScreenName)
    {
        if (sampler.lock()->isSoundNameOccupied(nameScreenName))
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

void CopySoundScreen::displayNewName() const
{
    findField("newname")->setText(newName);
}

void CopySoundScreen::displaySnd() const
{
    if (!sampler.lock()->getSound())
    {
        return;
    }

    findField("snd")->setText(sampler.lock()->getSound()->getName());
}

void CopySoundScreen::setNewName(const std::string &s)
{
    newName = s;
    displayNewName();
}
