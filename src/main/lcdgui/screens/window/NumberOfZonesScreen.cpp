#include "NumberOfZonesScreen.hpp"

#include <ui/sampler/SoundGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

NumberOfZonesScreen::NumberOfZonesScreen(const int& layer)
	: ScreenComponent("numberofzones", layer)
{
}

void NumberOfZonesScreen::open()
{
	displayNumberOfZones();
}

void NumberOfZonesScreen::displayNumberOfZones()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("numberofzones").lock()->setText(to_string(soundGui->getNumberOfZones()));
}

void NumberOfZonesScreen::function(int i)
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	auto sound = sampler.lock()->getSound().lock();
	switch (i)
	{
	case 4:
		soundGui->initZones(sound->getLastFrameIndex());
		ls.lock()->openScreen("zone");
		break;
	}
}

void NumberOfZonesScreen::turnWheel(int i)
{
	init();
	if (param.compare("") == 0)
	{
		return;
	}
	else if (param.compare("numberofzones") == 0)
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		soundGui->setNumberOfZones(soundGui->getNumberOfZones() + i);
		displayNumberOfZones();
	}
}

