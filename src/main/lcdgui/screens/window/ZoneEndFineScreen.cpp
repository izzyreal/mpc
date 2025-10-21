#include "ZoneEndFineScreen.hpp"
#include "hardware/Hardware.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
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
	auto zoneScreen = mpc.screens->get<ZoneScreen>();
	auto trimScreen = mpc.screens->get<TrimScreen>();

	auto sound = sampler->getSound();

	if (!sound)
		return;

	findWave()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave()->setCenterSamplePos(zoneScreen->getZoneEnd(zoneScreen->zone));
}

void ZoneEndFineScreen::displayEnd()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>();
	findField("end")->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayLngthLabel()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>();
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
	auto zoneScreen = mpc.screens->get<ZoneScreen>();

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
    command::SplitLeftCommand(mpc).execute();
}

void ZoneEndFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void ZoneEndFineScreen::pressEnter()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>();
	zoneScreen->pressEnter();

	displayEnd();
	displayLngthLabel();
	displayFineWave();
}

void ZoneEndFineScreen::setSlider(int i)
{
    if (!mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        return;
    }

    init();

    if (param == "end")
    {
        auto zoneScreen = mpc.screens->get<ZoneScreen>();
        zoneScreen->setSlider(i);
        displayEnd();
        displayLngthLabel();
        displayFineWave();
    }
}
