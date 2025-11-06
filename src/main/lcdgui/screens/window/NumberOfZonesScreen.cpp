#include "NumberOfZonesScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/ZoneScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

NumberOfZonesScreen::NumberOfZonesScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "number-of-zones", layerIndex)
{
}

void NumberOfZonesScreen::open()
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    numberOfZones = zoneScreen->numberOfZones;
    displayNumberOfZones();
}

void NumberOfZonesScreen::displayNumberOfZones()
{
    findField("number-of-zones")->setTextPadded(numberOfZones);
}

void NumberOfZonesScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ZoneScreen);
            break;
        case 4:
            const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
            auto sound = sampler->getSound();
            zoneScreen->numberOfZones = numberOfZones;
            zoneScreen->initZones();
            openScreenById(ScreenId::ZoneScreen);
            break;
    }
}

void NumberOfZonesScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "number-of-zones")
    {
        setNumberOfZones(numberOfZones + i);
    }
}

void NumberOfZonesScreen::setNumberOfZones(const int i)
{
    numberOfZones = std::clamp(i, 1, 16);
    displayNumberOfZones();
}
