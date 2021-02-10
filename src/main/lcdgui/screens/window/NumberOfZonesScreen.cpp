#include "NumberOfZonesScreen.hpp"

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/ZoneScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

NumberOfZonesScreen::NumberOfZonesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "number-of-zones", layerIndex)
{
}

void NumberOfZonesScreen::open()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	numberOfZones = zoneScreen->numberOfZones;
	displayNumberOfZones();
}

void NumberOfZonesScreen::displayNumberOfZones()
{
	findField("number-of-zones").lock()->setTextPadded(numberOfZones);
}

void NumberOfZonesScreen::function(int i)
{
	switch (i)
	{
	case 4:
		auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
		auto sound = sampler.lock()->getSound().lock();
		zoneScreen->numberOfZones = numberOfZones;
		zoneScreen->initZones();
		openScreen("zone");
		break;
	}
}

void NumberOfZonesScreen::turnWheel(int i)
{
	init();
	if (param.compare("number-of-zones") == 0)
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
