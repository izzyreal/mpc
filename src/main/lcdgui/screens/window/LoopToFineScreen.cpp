#include "LoopToFineScreen.hpp"
#include "Mpc.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/screens/LoopScreen.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

LoopToFineScreen::LoopToFineScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop-to-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void LoopToFineScreen::open()
{
    findField("loop-lngth")->setAlignment(Alignment::Centered);
    displayTo();
    findField("to")->enableTwoDots();
    displayLngthField();
    displayLoopLngth();
    findField("lngth")->enableTwoDots();

    displayPlayX();
    displayFineWave();
}

void LoopToFineScreen::displayLoopLngth()
{
    auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();
    findField("loop-lngth")->setText(loopScreen->loopLngthFix ? "FIX" : "VARI");
}

void LoopToFineScreen::displayLngthField()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findField("lngth")->setTextPadded(sound->getEnd() - sound->getLoopTo(),
                                      " ");
}

void LoopToFineScreen::displayFineWave()
{
    auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findWave()->setSampleData(sound->getSampleData(), sound->isMono(),
                              trimScreen->view);
    findWave()->setCenterSamplePos(sound->getLoopTo());
}

void LoopToFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void LoopToFineScreen::displayTo()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findField("to")->setTextPadded(sound->getLoopTo(), " ");
}

void LoopToFineScreen::function(int i)
{

    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            findWave()->zoomPlus();
            break;
        case 2:
            findWave()->zoomMinus();
            break;
        case 4:
            sampler->playX();
            break;
    }
}

void LoopToFineScreen::turnWheel(int i)
{
    auto sound = sampler->getSound();
    auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();

    auto soundInc = getSoundIncrement(i);

    const auto focusedField = getFocusedFieldOrThrow();

    if (focusedField->isSplit())
    {
        soundInc = focusedField->getSplitIncrement(i >= 0);
    }

    if (focusedField->isTypeModeEnabled())
    {
        focusedField->disableTypeMode();
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "loop-lngth")
    {
        loopScreen->loopLngthFix = i > 0;
        displayLoopLngth();
    }
    else if (focusedFieldName == "lngth")
    {
        auto newLength = (sound->getEnd() - sound->getLoopTo()) + soundInc;

        loopScreen->setLength(newLength);

        displayTo();
        displayLngthField();
        displayFineWave();
    }
    else if (focusedFieldName == "to")
    {
        loopScreen->setLoopTo(sound->getLoopTo() + soundInc);

        displayTo();
        displayLngthField();
        displayFineWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler->setPlayX(sampler->getPlayX() + i);
        displayPlayX();
    }
}

void LoopToFineScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void LoopToFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void LoopToFineScreen::pressEnter()
{
    auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();
    loopScreen->pressEnter();
    displayTo();
    displayLngthField();
    displayFineWave();
}

void LoopToFineScreen::setSlider(int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "to")
    {
        loopScreen->setSliderLoopTo(i);
        displayTo();
        displayLngthField();
        displayFineWave();
    }
    else if (focusedFieldName == "lngth")
    {
        loopScreen->setSliderLength(i);
        displayTo();
        displayLngthField();
        displayFineWave();
    }
}
