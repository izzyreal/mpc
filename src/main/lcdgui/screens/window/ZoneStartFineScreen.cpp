#include "ZoneStartFineScreen.hpp"

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ZoneStartFineScreen::ZoneStartFineScreen(const int layerIndex)
	: ScreenComponent("zone-start-fine", layerIndex)
{
}

void ZoneStartFineScreen::open()
{
	addChild(move(make_shared<Wave>()));
	addChild(move(make_shared<TwoDots>()));

	displayStart();
	displayLngthLabel();

	displayPlayX();
	displayFineWaveform();

	findTwoDots().lock()->setVisible(0, false);
	findTwoDots().lock()->setVisible(1, false);
	findTwoDots().lock()->setVisible(2, true);
	findTwoDots().lock()->setVisible(3, true);
	findTwoDots().lock()->setSelected(3, false);
}

void ZoneStartFineScreen::displayFineWaveform()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));

	auto sound = sampler.lock()->getSound().lock();
	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(zoneScreen->getZoneStart(zoneScreen->zone));
}

void ZoneStartFineScreen::displayStart()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));
	findField("start").lock()->setTextPadded(zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayLngthLabel()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));
	findLabel("lngth").lock()->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone) - zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneStartFineScreen::function(int i)
{
	BaseControls::function(i);

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

void ZoneStartFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto loopLength = static_cast<int>((sound->getEnd() - sound->getLoopTo()));
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));

	auto sampleLength = sound->getFrameCount();

	if (param.compare("start") == 0)
	{
		zoneScreen->setZoneStart(zoneScreen->zone, zoneScreen->getZoneStart(zoneScreen->zone) + i);
		displayStart();
		displayLngthLabel();
		displayFineWaveform();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}
