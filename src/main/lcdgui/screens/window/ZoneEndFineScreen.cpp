#include "ZoneEndFineScreen.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
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
    mpc.getControls().lock()->getControls()->typableParams = { "end" };

    displayEnd();
	findField("end").lock()->enableTwoDots();
	displayLngthLabel();
	findLabel("lngth").lock()->enableTwoDots();

	displayPlayX();
	displayFineWave();
}

void ZoneEndFineScreen::displayFineWave()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(zoneScreen->getZoneEnd(zoneScreen->zone));
}

void ZoneEndFineScreen::displayEnd()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	findField("end").lock()->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayLngthLabel()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	findLabel("lngth").lock()->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone) - zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneEndFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneEndFineScreen::function(int i)
{
	ScreenComponent::function(i);

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
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");

	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();

	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param.compare("end") == 0)
	{
		zoneScreen->setZoneEnd(zoneScreen->zone, zoneScreen->getZoneEnd(zoneScreen->zone) + soundInc);
		displayLngthLabel();
		displayEnd();
		displayFineWave();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}

void ZoneEndFineScreen::left()
{
	splitLeft();
}

void ZoneEndFineScreen::right()
{
	splitRight();
}

void ZoneEndFineScreen::pressEnter()
{
	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	zoneScreen->pressEnter();

	displayEnd();
	displayLngthLabel();
	displayFineWave();
}
