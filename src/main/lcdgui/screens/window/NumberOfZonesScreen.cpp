#include "NumberOfZonesScreen.hpp"

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/ZoneScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

NumberOfZonesScreen::NumberOfZonesScreen(const int layerIndex)
	: ScreenComponent("number-of-zones", layerIndex)
{
}

void NumberOfZonesScreen::open()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));
	numberOfZones = zoneScreen->numberOfZones;
	displayNumberOfZones();
}

void NumberOfZonesScreen::displayNumberOfZones()
{
	findField("number-of-zones").lock()->setText(to_string(numberOfZones));
}

void NumberOfZonesScreen::function(int i)
{
	switch (i)
	{
	case 4:
		auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));
		auto sound = sampler.lock()->getSound().lock();
		zoneScreen->numberOfZones = numberOfZones;
		zoneScreen->initZones(sound->getLastFrameIndex());
		ls.lock()->openScreen("zone");
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
