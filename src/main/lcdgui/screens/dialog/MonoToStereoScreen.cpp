#include "MonoToStereoScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

#include <memory>

#include "StrUtil.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "sampler/Sampler.hpp"

namespace mpc::lcdgui
{
    class FunctionKey;
}

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;

MonoToStereoScreen::MonoToStereoScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "mono-to-stereo", layerIndex)
{
}

void MonoToStereoScreen::open()
{
    if (sampler->getSound() &&
        ls->isPreviousScreenNot({ScreenId::NameScreen, ScreenId::PopupScreen}))
    {
        auto name = sampler->getSound()->getName();
        name = StrUtil::trim(name);
        name = StrUtil::padRight(name, "_", 16);
        name = name.substr(0, 14);
        newStName = name + "-S";
    }

    if (ls->isPreviousScreenNot({ScreenId::NameScreen, ScreenId::PopupScreen}))
    {
        ls->setFocus("lsource");
    }

    setRSource(sampler->getSoundIndex());
    displayLSource();
    displayNewStName();
}

void MonoToStereoScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "lsource" && i < 0)
    {
        sampler->selectPreviousSound();
        displayLSource();
    }
    else if (focusedFieldName == "lsource" && i > 0)
    {
        sampler->selectNextSound();
        displayLSource();
    }
    else if (focusedFieldName == "rsource")
    {
        auto nextSoundIndex = rSource;
        if (i > 0 && nextSoundIndex != sampler->getSoundCount() - 1)
        {
            nextSoundIndex++;
        }
        else if (i < 0 && nextSoundIndex != 0)
        {
            nextSoundIndex--;
        }

        setRSource(nextSoundIndex);
        displayRSource();
    }
}

void MonoToStereoScreen::openNameScreen()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "newstname")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            newStName = nameScreenName;
            openScreenById(ScreenId::MonoToStereoScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(newStName, 16, enterAction, "mono-to-stereo");
        openScreenById(ScreenId::NameScreen);
    }
}

void MonoToStereoScreen::function(const int j)
{
    switch (j)
    {
        case 3:
            openScreenById(ScreenId::SoundScreen);
            break;
        case 4:
        {
            const auto right = sampler->getSortedSounds()[rSource].first;
            if (!sampler->getSound()->isMono() || !right->isMono())
            {
                return;
            }

            for (const auto &s : sampler->getSounds())
            {
                if (s->getName() == newStName)
                {
                    ls->showPopupAndAwaitInteraction("Name already used");
                    return;
                }
            }

            const auto left = sampler->getSound();

            const auto newSampleDataRight =
                std::make_shared<std::vector<float>>();

            if (right->getSampleRate() > left->getSampleRate())
            {
                newSampleDataRight->resize(left->getSampleData()->size());

                for (int i = 0; i < newSampleDataRight->size(); i++)
                {
                    (*newSampleDataRight)[i] = (*right->getSampleData())[i];
                }
            }
            else
            {
                *newSampleDataRight = *right->getSampleData();
            }

            const auto newSound = sampler->addSound(left->getSampleRate());

            if (newSound == nullptr)
            {
                return;
            }

            newSound->setName(newStName);
            sampler::Sampler::mergeToStereo(left->getSampleData(),
                                            newSampleDataRight,
                                            newSound->getMutableSampleData());
            newSound->setMono(false);
            openScreenById(ScreenId::SoundScreen);
        }
    }
}

void MonoToStereoScreen::displayLSource()
{
    if (!sampler->getSound())
    {
        return;
    }

    findField("lsource")->setText(sampler->getSound()->getName());

    if (sampler->getSound()->isMono() && sampler->getSound()->isMono())
    {
        ls->setFunctionKeysArrangement(0);
    }
    else
    {
        ls->setFunctionKeysArrangement(1);
        findChild<Background>("")->repaintUnobtrusive(
            findChild<FunctionKey>("fk4")->getRect());
    }
}

void MonoToStereoScreen::displayRSource()
{
    if (rSource >= sampler->getSoundCount())
    {
        findField("rsource")->setText("");
        return;
    }

    const auto sound = sampler->getSortedSounds()[rSource];

    findField("rsource")->setText(sound.first->getName());

    if (sound.first->isMono() && sampler->getSound()->isMono())
    {
        ls->setFunctionKeysArrangement(0);
    }
    else
    {
        ls->setFunctionKeysArrangement(1);
        findBackground()->repaintUnobtrusive(
            findChild<FunctionKey>("fk4")->getRect());
    }
}

void MonoToStereoScreen::displayNewStName() const
{
    findField("newstname")->setText(newStName);
}

void MonoToStereoScreen::setRSource(const int i)
{
    rSource = std::clamp(i, 0, std::max(0, sampler->getSoundCount() - 1));
    displayRSource();
}
