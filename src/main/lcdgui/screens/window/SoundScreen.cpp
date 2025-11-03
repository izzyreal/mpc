#include "SoundScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

SoundScreen::SoundScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sound", layerIndex)
{
}

void SoundScreen::open()
{
    displaySoundName();
    displayType();
    displayRate();
    displaySize();
}

void SoundScreen::openNameScreen()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "soundname")
    {
        const auto enterAction = [this](std::string &nameScreenName)
        {
            if (sampler->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            sampler->getSound()->setName(nameScreenName);
            openScreenById(ScreenId::SoundScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(sampler->getSound()->getName(), 16, enterAction,
                               "sound");
        openScreenById(ScreenId::NameScreen);
    }
}

void SoundScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            openScreenById(ScreenId::DeleteSoundScreen);
            break;
        case 2:
            openScreenById(ScreenId::ConvertSoundScreen);
            break;
        case 4:
            openScreenById(ScreenId::CopySoundScreen);
            break;
    }
}

void SoundScreen::displaySoundName()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findField("soundname")->setText("");
        return;
    }

    findField("soundname")->setText(sound->getName());
}

void SoundScreen::displayType()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findLabel("type")->setText("");
        return;
    }

    findLabel("type")->setText(
        "Type:" + std::string(sound->isMono() ? "MONO" : "STEREO"));
}

void SoundScreen::displayRate()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findLabel("rate")->setText("");
        return;
    }

    findLabel("rate")->setText(
        "Rate: " + std::to_string(sound->getSampleRate()) + "Hz");
}

void SoundScreen::displaySize()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findLabel("size")->setText("");
        return;
    }

    findLabel("size")->setText(
        "Size:" +
        StrUtil::padLeft(std::to_string(sound->getSampleData()->size() / 500),
                         " ", 4) +
        "kbytes");
}
