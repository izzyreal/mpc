#include "ZoneEndFineScreen.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

ZoneEndFineScreen::ZoneEndFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "zone-end-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void ZoneEndFineScreen::open()
{
    displayEnd();
	findField("end")->enableTwoDots();
	displayLngthLabel();
	findLabel("lngth")->enableTwoDots();

	displayPlayX();
	displayFineWave();
}

void ZoneEndFineScreen::displayFineWave()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

	auto sound = sampler->getSound();

	if (!sound)
		return;

	findWave()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave()->setCenterSamplePos(zoneScreen->getZoneEnd(zoneScreen->zone));
}

void ZoneEndFineScreen::displayEnd()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	findField("end")->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayLngthLabel()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	findLabel("lngth")->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone) - zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void ZoneEndFineScreen::function(int i)
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

void ZoneEndFineScreen::turnWheel(int i)
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

	if (param == "end")
	{
		zoneScreen->setZoneEnd(zoneScreen->zone, zoneScreen->getZoneEnd(zoneScreen->zone) + soundInc);
		displayLngthLabel();
		displayEnd();
		displayFineWave();
	}
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
}

void ZoneEndFineScreen::left()
{
    BaseControls::splitLeft(mpc);
}

void ZoneEndFineScreen::right()
{
    BaseControls::splitRight(mpc);
}

void ZoneEndFineScreen::pressEnter()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	zoneScreen->pressEnter();

	displayEnd();
	displayLngthLabel();
	displayFineWave();
}

void ZoneEndFineScreen::setSlider(int i)
{
    if (!mpc.getControls()->isShiftPressed())
    {
        return;
    }

    init();

    if (param == "end")
    {
        auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
        zoneScreen->setSlider(i);
        displayEnd();
        displayLngthLabel();
        displayFineWave();
    }
}
