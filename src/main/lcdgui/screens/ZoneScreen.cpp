#include "ZoneScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/window/NumberOfZonesScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

ZoneScreen::ZoneScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "zone", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);

	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(false);
	baseControls->typableParams = vector<string>{ "st", "end" };
}

void ZoneScreen::open()
{
	if (zones.empty())
		initZones();

	bool sound = sampler.lock()->getSound().lock() ? true : false;
	findField("snd").lock()->setFocusable(sound);
	findField("playx").lock()->setFocusable(sound);
	findField("st").lock()->setFocusable(sound);
	findField("st").lock()->enableTwoDots();
	findField("end").lock()->setFocusable(sound);
	findField("end").lock()->enableTwoDots();
	findField("zone").lock()->setFocusable(sound);
	findField("dummy").lock()->setFocusable(!sound);

	displayWave();
	displaySnd();
	displayPlayX();
	displaySt();
	displayEnd();
	displayZone();
}

void ZoneScreen::openWindow()
{
	init();

	if (param.compare("snd") == 0)
	{
		sampler.lock()->setPreviousScreenName("zone");
		ls.lock()->openScreen("sound");
	}
	else if (param.compare("zone") == 0)
	{
		ls.lock()->openScreen("number-of-zones");
	}
	else if (param.compare("st") == 0)
	{
		ls.lock()->openScreen("zone-start-fine");
	}
	else if (param.compare("end") == 0)
	{
		ls.lock()->openScreen("zone-end-fine");
	}
}

void ZoneScreen::function(int f)
{
	init();

	switch (f)
	{
	case 0:
		ls.lock()->openScreen("trim");
		break;
	case 1:
		ls.lock()->openScreen("loop");
		break;
	case 2:
		sampler.lock()->sort();
		break;
	case 3:
		ls.lock()->openScreen("params");
		break;
	case 4:
	{
		if (sampler.lock()->getSoundCount() == 0)
		{
			return;
		}

		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(mpc.screens->getScreenComponent("edit-sound"));
		editSoundScreen->setPreviousScreenName("zone");
		ls.lock()->openScreen("edit-sound");
		break;
	}
	case 5:
	{
		if (mpc.getControls().lock()->isF6Pressed())
		{
			return;
		}

		mpc.getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
		break;
	}
	}
}

void ZoneScreen::turnWheel(int i)
{
	init();

	if (param == "")
	{
		return;
	}
	
	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();
	
	if (field->isSplit())
	{
		soundInc = i >= 0 ? splitInc[field->getActiveSplit()] : -splitInc[field->getActiveSplit()];
	}
	
	if (param.compare("st") == 0)
	{
		setZoneStart(zone, getZoneStart(zone) + soundInc);
		displaySt();
		displayWave();
	}
	else if (param.compare("end") == 0)
	{
		setZoneEnd(zone, getZoneEnd(zone) + soundInc);
		displayEnd();
		displayWave();
	}
	else if (param.compare("zone") == 0)
	{
		setZone(zone + i);
		displayZone();
		displaySt();
		displayEnd();
		displayWave();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->selectNextSound();
		displayEnd();
		displaySnd();
		displaySt();
		displayWave();
		displayZone();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->selectPreviousSound();
		displayEnd();
		displaySnd();
		displaySt();
		displayWave();
		displayZone();
	}
}

void ZoneScreen::displayWave()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findWave().lock()->setSampleData(nullptr, true, 0);
		findWave().lock()->setSelection(0, 0);
		return;
	}

	auto sampleData = sound->getSampleData();
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
	findWave().lock()->setSampleData(sampleData, sampler.lock()->getSound().lock()->isMono(), trimScreen->view);
	findWave().lock()->setSelection(getZoneStart(zone), getZoneEnd(zone));
}

void ZoneScreen::displaySnd()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findField("snd").lock()->setText("(no sound)");
		ls.lock()->setFocus("dummy");
		return;
	}

	if (ls.lock()->getFocus().compare("dummy") == 0)
	{
		ls.lock()->setFocus(findField("snd").lock()->getName());
	}

	auto sampleName = sound->getName();

	if (!sound->isMono())
	{
		sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";
	}

	findField("snd").lock()->setText(sampleName);
}

void ZoneScreen::displayPlayX()
{
	findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneScreen::displaySt()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		findField("st").lock()->setTextPadded(getZoneStart(zone), " ");
	}
	else {
		findField("st").lock()->setText("       0");
	}
}

void ZoneScreen::displayEnd()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		findField("end").lock()->setTextPadded(getZoneEnd(zone), " ");
	}
	else {
		findField("end").lock()->setText("       0");
	}
}

void ZoneScreen::displayZone()
{
	if (sampler.lock()->getSoundCount() == 0)
	{
		findField("zone").lock()->setText("1");
		return;
	}

	findField("zone").lock()->setText(to_string(zone + 1));
}

void ZoneScreen::initZones()
{
	zones.clear();
	
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		zone = 0;
		return;
	}

	int zoneLength = (int)floor(sound->getFrameCount() / numberOfZones);
	int zoneStart = 0;

	for (int i = 0; i < numberOfZones - 1; i++)
	{
		zones.push_back(vector<int>(2));
		zones[i][0] = zoneStart;
		zones[i][1] = zoneStart + zoneLength - 1;
		zoneStart += zoneLength;
	}

	zones.push_back(vector<int>(2));
	zones[numberOfZones - 1][0] = zoneStart;
	zones[numberOfZones - 1][1] = sound->getFrameCount();
	zone = 0;
}

void ZoneScreen::setZoneStart(int zoneIndex, int start)
{
	if (start > zones[zoneIndex][1]) {
		start = zones[zoneIndex][1];
	}
	
	if (zoneIndex == 0 && start < 0) {
		start = 0;
	}
	
	if (zoneIndex > 0 && start < zones[zoneIndex - 1][0]) {
		start = zones[zoneIndex - 1][0];
	}
	
	zones[zoneIndex][0] = start;
	
	if (zoneIndex != 0)
	{
		zones[zoneIndex - 1][1] = start;
	}
	
	displaySt();
	displayWave();
}

int ZoneScreen::getZoneStart(int zoneIndex)
{
	if (zoneIndex >= zones.size())
	{
		return 0;
	}

	return zones[zoneIndex][0];
}

void ZoneScreen::setZoneEnd(int zoneIndex, int end)
{
	auto length = sampler.lock()->getSound().lock()->getFrameCount();

	if (end < zones[zoneIndex][0]) {
		end = zones[zoneIndex][0];
	}
	if (zoneIndex < numberOfZones - 1 && end > zones[zoneIndex + 1][1]) {
		end = zones[zoneIndex + 1][1];
	}
	if (zoneIndex == numberOfZones - 1 && end > length) {
		end = length;
	}
	zones[zoneIndex][1] = end;
	if (zoneIndex != numberOfZones - 1) {
		zones[zoneIndex + 1][0] = end;
	}
	displayEnd();
	displayWave();
}

int ZoneScreen::getZoneEnd(int zoneIndex)
{
	if (zoneIndex >= zones.size())
	{
		return 0;
	}

	return zones[zoneIndex][1];
}

void ZoneScreen::setZone(int i)
{
	if (i < 0 || i >= numberOfZones)
	{
		return;
	}

	zone = i;

	displayWave();
	displaySt();
	displayEnd();
	displayZone();
}

vector<int> ZoneScreen::getZone()
{
	return vector<int>{ getZoneStart(zone), getZoneEnd(zone) };
}
