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
    zoneCount = zoneScreen->getZoneCount();
    displayNumberOfZones();
}

void NumberOfZonesScreen::displayNumberOfZones() const
{
    findField("number-of-zones")->setTextPadded(zoneCount);
}

void NumberOfZonesScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ZoneScreen);
            break;
        case 4:
        {
            const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
            auto sound = sampler->getSound();
            zoneScreen->setZoneCount(zoneCount);
            zoneScreen->initZones();
            openScreenById(ScreenId::ZoneScreen);
            break;
        }
        default:;
    }
}

void NumberOfZonesScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "number-of-zones")
    {
        setNumberOfZones(zoneCount + i);
    }
}

void NumberOfZonesScreen::setNumberOfZones(const int i)
{
    zoneCount = std::clamp(i, 1, 16);
    displayNumberOfZones();
}
