#include "StereoToMonoScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"

namespace mpc::lcdgui
{
    class FunctionKey;
}

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;

StereoToMonoScreen::StereoToMonoScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "stereo-to-mono", layerIndex)
{
}

void StereoToMonoScreen::open()
{
    if (ls->isPreviousScreenNot({ScreenId::NameScreen, ScreenId::PopupScreen}))
    {
        updateNewNames();
        ls->setFocus("stereosource");
    }

    displayNewLName();
    displayNewRName();
    displayStereoSource();
}

void StereoToMonoScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "stereosource")
    {
        sampler->nudgeSoundIndex(i > 0);
        displayStereoSource();
    }
}

void StereoToMonoScreen::openNameScreen()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "newlname" || focusedFieldName == "newrname")
    {
        const auto isL = focusedFieldName == "newlname";

        const auto enterAction = [this, isL](std::string &nameScreenName)
        {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            if (isL)
            {
                setNewLName(nameScreenName);
            }
            else
            {
                setNewRName(nameScreenName);
            }
            openScreenById(ScreenId::StereoToMonoScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        const auto newName = isL ? newLName : newRName;
        nameScreen->initialize(newName, 16, enterAction, "stereo-to-mono");
        openScreenById(ScreenId::NameScreen);
    }
}

void StereoToMonoScreen::function(int i)
{

    switch (i)
    {
        case 3:
            ls->closeCurrentScreen();
            break;
        case 4:
        {
            const auto sound = sampler->getSound();

            if (sound->isMono())
            {
                return;
            }

            for (const auto &s : sampler->getSounds())
            {
                if (s->getName() == newLName || s->getName() == newRName)
                {
                    ls->showPopupAndAwaitInteraction("Name already used");
                    return;
                }
            }

            const auto left = sampler->addSound(sound->getSampleRate());

            if (left == nullptr)
            {
                return;
            }

            const auto right = sampler->addSound(sound->getSampleRate());

            if (right == nullptr)
            {
                sampler->deleteSound(left);
                return;
            }

            left->setName(newLName);
            right->setName(newRName);

            left->setMono(true);
            right->setMono(true);

            const auto leftData = left->getMutableSampleData();
            const auto rightData = right->getMutableSampleData();

            for (int frameIndex = 0; frameIndex < sound->getFrameCount();
                 frameIndex++)
            {
                leftData->push_back((*sound->getSampleData())[frameIndex]);
                rightData->push_back(
                    (*sound->getSampleData())[frameIndex +
                                              sound->getFrameCount()]);
            }

            left->setEnd(left->getSampleData()->size());
            right->setEnd(right->getSampleData()->size());
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        }
    }
}

void StereoToMonoScreen::updateNewNames()
{
    if (!sampler->getSound() || sampler->getSound()->isMono())
    {
        return;
    }

    auto name = sampler->getSound()->getName();
    name = StrUtil::trim(name);
    name = StrUtil::padRight(name, "_", 16);
    name = name.substr(0, 14);

    setNewLName(name + "-L");
    setNewRName(name + "-R");
}

void StereoToMonoScreen::displayStereoSource()
{
    const auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findField("stereosource")->setText(sound->getName());

    if (sound->isMono())
    {
        ls->setFunctionKeysArrangement(1);
        findBackground()->repaintUnobtrusive(
            findChild<FunctionKey>("fk4")->getRect());
    }
    else
    {
        ls->setFunctionKeysArrangement(0);
    }
}

void StereoToMonoScreen::displayNewLName()
{
    findField("newlname")->setText(newLName);
}

void StereoToMonoScreen::displayNewRName()
{
    findField("newrname")->setText(newRName);
}

void StereoToMonoScreen::setNewLName(const std::string &s)
{
    newLName = s;
    displayNewLName();
}

void StereoToMonoScreen::setNewRName(const std::string &s)
{
    newRName = s;
    displayNewRName();
}
