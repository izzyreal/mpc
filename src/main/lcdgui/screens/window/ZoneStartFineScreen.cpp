#include "ZoneStartFineScreen.hpp"
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

ZoneStartFineScreen::ZoneStartFineScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "zone-start-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void ZoneStartFineScreen::open()
{
    findField("start")->enableTwoDots();
    findLabel("lngth")->enableTwoDots();
    displayStart();
    displayLngthLabel();

    displayPlayX();
    displayFineWave();
}

void ZoneStartFineScreen::displayFineWave()
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
        zoneScreen->getZoneStart(zoneScreen->getSelectedZoneIndex()));
}

void ZoneStartFineScreen::displayStart() const
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    findField("start")->setTextPadded(
        zoneScreen->getZoneStart(zoneScreen->getSelectedZoneIndex()), " ");
}

void ZoneStartFineScreen::displayLngthLabel() const
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    findLabel("lngth")->setTextPadded(
        zoneScreen->getZoneEnd(zoneScreen->getSelectedZoneIndex()) -
            zoneScreen->getZoneStart(zoneScreen->getSelectedZoneIndex()),
        " ");
}

void ZoneStartFineScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneStartFineScreen::function(const int i)
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

void ZoneStartFineScreen::turnWheel(const int i)
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
        focusedFieldName == "start")
    {
        zoneScreen->setZoneStart(
            zoneScreen->getSelectedZoneIndex(),
            zoneScreen->getZoneStart(zoneScreen->getSelectedZoneIndex()) +
                soundInc);
        displayStart();
        displayLngthLabel();
        displayFineWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
}

void ZoneStartFineScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void ZoneStartFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void ZoneStartFineScreen::pressEnter()
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    zoneScreen->pressEnter();

    displayStart();
    displayLngthLabel();
    displayFineWave();
}

void ZoneStartFineScreen::setSlider(const int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "start")
    {
        const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
        zoneScreen->setSliderZoneStart(i);
        displayStart();
        displayLngthLabel();
        displayFineWave();
    }
}
