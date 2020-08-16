#include "ZoneStartFineScreen.hpp"

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::controls;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ZoneStartFineScreen::ZoneStartFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "zone-start-fine", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
	baseControls->typableParams = { "start" };

	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void ZoneStartFineScreen::open()
{
	findField("start").lock()->enableTwoDots();
	findLabel("lngth").lock()->enableTwoDots();
	displayStart();
	displayLngthLabel();


	displayPlayX();
	displayFineWave();
}

void ZoneStartFineScreen::displayFineWave()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(zoneScreen->getZoneStart(zoneScreen->zone));
}

void ZoneStartFineScreen::displayStart()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	findField("start").lock()->setTextPadded(zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayLngthLabel()
{
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	findLabel("lngth").lock()->setTextPadded(zoneScreen->getZoneEnd(zoneScreen->zone) - zoneScreen->getZoneStart(zoneScreen->zone), " ");
}

void ZoneStartFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneStartFineScreen::function(int i)
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

void ZoneStartFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto loopLength = static_cast<int>((sound->getEnd() - sound->getLoopTo()));
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));

	auto sampleLength = sound->getFrameCount();

	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();

	if (field->isSplit())
		soundInc = i >= 0 ? splitInc[field->getActiveSplit()] : -splitInc[field->getActiveSplit()];

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param.compare("start") == 0)
	{
		zoneScreen->setZoneStart(zoneScreen->zone, zoneScreen->getZoneStart(zoneScreen->zone) + soundInc);
		displayStart();
		displayLngthLabel();
		displayFineWave();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
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
	auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(mpc.screens->getScreenComponent("zone"));
	zoneScreen->pressEnter();

	displayStart();
	displayLngthLabel();
	displayFineWave();
}