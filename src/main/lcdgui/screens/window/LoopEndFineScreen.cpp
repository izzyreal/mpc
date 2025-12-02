#include "LoopEndFineScreen.hpp"
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

LoopEndFineScreen::LoopEndFineScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop-end-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void LoopEndFineScreen::open()
{
    findField("loop-lngth")->setAlignment(Alignment::Centered);
    findField("end")->enableTwoDots();
    displayEnd();
    displayLngthField();
    findField("lngth")->enableTwoDots();
    displayLoopLngth();

    displayPlayX();
    displayFineWave();
}

void LoopEndFineScreen::displayLoopLngth() const
{
    const auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();
    findField("loop-lngth")->setText(loopScreen->isLoopLengthFixed() ? "FIX" : "VARI");
}

void LoopEndFineScreen::displayLngthField() const
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        return;
    }

    findField("lngth")->setTextPadded(sound->getEnd() - sound->getLoopTo(),
                                      " ");
}

void LoopEndFineScreen::displayFineWave()
{
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();

    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        return;
    }

    findWave()->setSampleData(sound->getSampleData(), sound->isMono(),
                              trimScreen->getView());
    findWave()->setCenterSamplePos(sound->getEnd());
}

void LoopEndFineScreen::displayEnd() const
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        return;
    }

    findField("end")->setTextPadded(sound->getEnd(), " ");
}

void LoopEndFineScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void LoopEndFineScreen::function(const int i)
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
            sampler.lock()->playX();
            break;
        default:;
    }
}

void LoopEndFineScreen::turnWheel(const int i)
{
    const auto sound = sampler.lock()->getSound();
    const auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();

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
        loopScreen->setLoopLengthFixed(i > 0);
        displayLoopLngth();
    }
    else if (focusedFieldName == "lngth")
    {
        const auto newLength = sound->getEnd() - sound->getLoopTo() + soundInc;

        loopScreen->setLength(newLength);
        displayEnd();
        displayLngthField();
        displayFineWave();
    }
    else if (focusedFieldName == "end")
    {
        const auto newValue = sound->getEnd() + soundInc;
        trimScreen->setEnd(newValue);

        displayEnd();
        displayLngthField();
        displayFineWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
}

void LoopEndFineScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void LoopEndFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void LoopEndFineScreen::pressEnter()
{
    const auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();
    loopScreen->pressEnter();
    displayEnd();
    displayLngthField();
    displayFineWave();
}

void LoopEndFineScreen::setSlider(const int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    const auto loopScreen = mpc.screens->get<ScreenId::LoopScreen>();
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "end")
    {
        trimScreen->setSliderEnd(i);
        displayEnd();
        displayLngthField();
        displayFineWave();
    }
    else if (focusedFieldName == "lngth")
    {
        loopScreen->setSliderLength(i);
        displayEnd();
        displayLngthField();
        displayFineWave();
    }
}
