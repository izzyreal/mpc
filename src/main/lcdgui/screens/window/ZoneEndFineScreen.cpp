#include "ZoneEndFineScreen.hpp"
#include "Mpc.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Label.hpp"

#include "lcdgui/screens/ZoneScreen.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

ZoneEndFineScreen::ZoneEndFineScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "zone-end-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void ZoneEndFineScreen::open()
{
    displayEnd();
    findField("end")->enableTwoDots();
    displayLngthLabel();
    findLabel("lngth")->enableTwoDots();

    displayPlayX();
    displayFineWave();
}

void ZoneEndFineScreen::displayFineWave()
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();

    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        return;
    }

    findWave()->setSampleData(sound->getSampleData(), sound->isMono(),
                              trimScreen->getView());
    findWave()->setCenterSamplePos(
        zoneScreen->getZoneEnd(zoneScreen->getSelectedZoneIndex()));
}

void ZoneEndFineScreen::displayEnd() const
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    findField("end")->setTextPadded(
        zoneScreen->getZoneEnd(zoneScreen->getSelectedZoneIndex()), " ");
}

void ZoneEndFineScreen::displayLngthLabel() const
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    findLabel("lngth")->setTextPadded(
        zoneScreen->getZoneEnd(zoneScreen->getSelectedZoneIndex()) -
            zoneScreen->getZoneStart(zoneScreen->getSelectedZoneIndex()),
        " ");
}

void ZoneEndFineScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneEndFineScreen::function(const int i)
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

void ZoneEndFineScreen::turnWheel(const int i)
{
    auto sound = sampler.lock()->getSound();
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();

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

    if (const auto focusedFieldName = focusedField->getName();
        focusedFieldName == "end")
    {
        zoneScreen->setZoneEnd(
            zoneScreen->getSelectedZoneIndex(),
            zoneScreen->getZoneEnd(zoneScreen->getSelectedZoneIndex()) +
                soundInc);
        displayLngthLabel();
        displayEnd();
        displayFineWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
}

void ZoneEndFineScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void ZoneEndFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void ZoneEndFineScreen::pressEnter()
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    zoneScreen->pressEnter();

    displayEnd();
    displayLngthLabel();
    displayFineWave();
}

void ZoneEndFineScreen::setSlider(const int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "end")
    {
        const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
        zoneScreen->setSlider(i);
        displayEnd();
        displayLngthLabel();
        displayFineWave();
    }
}
