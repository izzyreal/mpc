#include "ZoneStartFineScreen.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/screens/ZoneScreen.hpp"
#include "lcdgui/screens/TrimScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

ZoneStartFineScreen::ZoneStartFineScreen(mpc::Mpc &mpc, const int layerIndex)
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
    auto zoneScreen = mpc.screens->get<ZoneScreen>();
    auto trimScreen = mpc.screens->get<TrimScreen>();

    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findWave()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
    findWave()->setCenterSamplePos(zoneScreen->getZoneStart(zoneScreen->zone));
}

void ZoneStartFineScreen::displayStart()
{
    auto zoneScreen = mpc.screens->get<ZoneScreen>();
    findField("start")->setTextPadded(zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayLngthLabel()
{
    auto zoneScreen = mpc.screens->get<ZoneScreen>();
    findLabel("lngth")->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone) - zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void ZoneStartFineScreen::function(int i)
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

void ZoneStartFineScreen::turnWheel(int i)
{
    auto sound = sampler->getSound();
    auto zoneScreen = mpc.screens->get<ZoneScreen>();

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

    if (focusedFieldName == "start")
    {
        zoneScreen->setZoneStart(zoneScreen->zone, zoneScreen->getZoneStart(zoneScreen->zone) + soundInc);
        displayStart();
        displayLngthLabel();
        displayFineWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler->setPlayX(sampler->getPlayX() + i);
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
    auto zoneScreen = mpc.screens->get<ZoneScreen>();
    zoneScreen->pressEnter();

    displayStart();
    displayLngthLabel();
    displayFineWave();
}

void ZoneStartFineScreen::setSlider(int i)
{
    if (!mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "start")
    {
        auto zoneScreen = mpc.screens->get<ZoneScreen>();
        zoneScreen->setSliderZoneStart(i);
        displayStart();
        displayLngthLabel();
        displayFineWave();
    }
}
