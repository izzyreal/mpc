#include "ZoneEndFineScreen.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ZoneEndFineScreen::ZoneEndFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "zone-end-fine", layerIndex)
{
	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void ZoneEndFineScreen::open()
{
	displayEnd();
	findField("end").lock()->enableTwoDots();
	displayLngthLabel();
	findLabel("lngth").lock()->enableTwoDots();

	displayPlayX();
	displayFineWaveform();
}

void ZoneEndFineScreen::displayFineWaveform()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(zoneScreen->getZoneEnd(zoneScreen->zone));
}

void ZoneEndFineScreen::displayEnd()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	findField("end").lock()->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayLngthLabel()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	findLabel("lngth").lock()->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone) - zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneEndFineScreen::function(int i)
{
	baseControls->function(i);

	switch (i)
	{
	case 1:
		findWave().lock()->zoomPlus();
		break;
	case 2:
		findWave().lock()->zoomMinus();
		break;
	case 4:
		sampler.lock()->playX();
		break;
	}
}

void ZoneEndFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));

	if (param.compare("end") == 0)
	{
		zoneScreen->setZoneEnd(zoneScreen->zone, zoneScreen->getZoneEnd(zoneScreen->zone) + i);
		displayLngthLabel();
		displayEnd();
		displayFineWaveform();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}
