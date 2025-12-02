#include "EndFineScreen.hpp"
#include "Mpc.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Label.hpp"

#include "lcdgui/screens/TrimScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

EndFineScreen::EndFineScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "end-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void EndFineScreen::open()
{
    findField("smpllngth")->setAlignment(Alignment::Centered);
    findField("end")->enableTwoDots();
    displayEnd();
    displaySmplLngth();
    displayLngthLabel();
    findLabel("lngth")->enableTwoDots();
    displayPlayX();
    displayFineWave();
}

void EndFineScreen::displayFineWave()
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

void EndFineScreen::displayEnd() const
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        return;
    }

    findField("end")->setTextPadded(sound->getEnd(), " ");
}

void EndFineScreen::displayLngthLabel() const
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        return;
    }

    findLabel("lngth")->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void EndFineScreen::displaySmplLngth() const
{
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
    findField("smpllngth")
        ->setText(trimScreen->isSampleLengthFixed() ? "FIX" : "VARI");
}

void EndFineScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void EndFineScreen::function(const int i)
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

void EndFineScreen::turnWheel(const int i)
{
    const auto sound = sampler.lock()->getSound();

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

    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "end")
    {
        const auto newValue = sound->getEnd() + soundInc;
        trimScreen->setEnd(newValue);

        displayLngthLabel();
        displayEnd();
        displayFineWave();
    }
    else if (focusedFieldName == "smpllngth")
    {
        trimScreen->setSampleLengthFixed(i > 0);
        displaySmplLngth();
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
}

void EndFineScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void EndFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void EndFineScreen::pressEnter()
{
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
    trimScreen->pressEnter();
    displayEnd();
    displayLngthLabel();
    displayFineWave();
}

void EndFineScreen::setSlider(const int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "end")
    {
        const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
        trimScreen->setSliderEnd(i);
        displayEnd();
        displayLngthLabel();
        displayFineWave();
    }
}
