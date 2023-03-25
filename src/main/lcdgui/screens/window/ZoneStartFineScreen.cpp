#include "ZoneStartFineScreen.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::controls;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

ZoneStartFineScreen::ZoneStartFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "zone-start-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void ZoneStartFineScreen::open()
{
    mpc.getControls()->getControls()->typableParams = { "start" };

    findField("start")->enableTwoDots();
	findLabel("lngth")->enableTwoDots();
	displayStart();
	displayLngthLabel();

	displayPlayX();
	displayFineWave();
}

void ZoneStartFineScreen::displayFineWave()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

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
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	findField("start")->setTextPadded(zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayLngthLabel()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
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
	init();
	auto sound = sampler->getSound();
    auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");

	auto soundInc = getSoundIncrement(i);
	auto field = findField(param);

	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param == "start")
	{
		zoneScreen->setZoneStart(zoneScreen->zone, zoneScreen->getZoneStart(zoneScreen->zone) + soundInc);
		displayStart();
		displayLngthLabel();
		displayFineWave();
	}
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
}

void ZoneStartFineScreen::left()
{
	splitLeft();
}

void ZoneStartFineScreen::right()
{
	splitRight();
}

void ZoneStartFineScreen::pressEnter()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	zoneScreen->pressEnter();

	displayStart();
	displayLngthLabel();
	displayFineWave();
}
