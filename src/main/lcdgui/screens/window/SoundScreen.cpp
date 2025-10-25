#include "SoundScreen.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"

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
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            sampler->getSound()->setName(nameScreenName);
            mpc.getLayeredScreen()->openScreen<SoundScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        nameScreen->initialize(sampler->getSound()->getName(), 16, enterAction, "sound");
        mpc.getLayeredScreen()->openScreen<NameScreen>();
    }
}

void SoundScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            mpc.getLayeredScreen()->openScreen<DeleteSoundScreen>();
            break;
        case 2:
            mpc.getLayeredScreen()->openScreen<ConvertSoundScreen>();
            break;
        case 4:
            mpc.getLayeredScreen()->openScreen<CopySoundScreen>();
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

    findLabel("type")->setText("Type:" + std::string(sound->isMono() ? "MONO" : "STEREO"));
}

void SoundScreen::displayRate()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findLabel("rate")->setText("");
        return;
    }

    findLabel("rate")->setText("Rate: " + std::to_string(sound->getSampleRate()) + "Hz");
}

void SoundScreen::displaySize()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findLabel("size")->setText("");
        return;
    }

    findLabel("size")->setText("Size:" + StrUtil::padLeft(std::to_string(sound->getSampleData()->size() / 500), " ", 4) + "kbytes");
}
