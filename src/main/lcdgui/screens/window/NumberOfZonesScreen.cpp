#include "NumberOfZonesScreen.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

NumberOfZonesScreen::NumberOfZonesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "number-of-zones", layerIndex)
{
}

void NumberOfZonesScreen::open()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>();
	numberOfZones = zoneScreen->numberOfZones;
	displayNumberOfZones();
}

void NumberOfZonesScreen::displayNumberOfZones()
{
	findField("number-of-zones")->setTextPadded(numberOfZones);
}

void NumberOfZonesScreen::function(int i)
{
	switch (i)
	{
    case 3:
        mpc.getLayeredScreen()->openScreen<ZoneScreen>();
        break;
	case 4:
		auto zoneScreen = mpc.screens->get<ZoneScreen>();
		auto sound = sampler->getSound();
		zoneScreen->numberOfZones = numberOfZones;
		zoneScreen->initZones();
        mpc.getLayeredScreen()->openScreen<ZoneScreen>();
		break;
	}
}

void NumberOfZonesScreen::turnWheel(int i)
{
	init();
	if (param == "number-of-zones")
	{
		setNumberOfZones(numberOfZones + i);
	}
}

void NumberOfZonesScreen::setNumberOfZones(int i)
{
	if (i < 1 || i > 16)
	{
		return;
	}
	numberOfZones = i;
	displayNumberOfZones();
}
