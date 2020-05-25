#include "ZoneScreen.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/window/NumberOfZonesScreen.hpp>

#include <ui/sampler/window/EditSoundGui.hpp>

#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

ZoneScreen::ZoneScreen(const int layerIndex) 
	: ScreenComponent("zone", layerIndex)
{
}

void ZoneScreen::open()
{
	typableParams = vector<string>{ "st", "end" };

	addChild(move(make_shared<Wave>()));
	addChild(move(make_shared<TwoDots>()));

	findTwoDots().lock()->setVisible(0, true);
	findTwoDots().lock()->setVisible(1, true);
	findTwoDots().lock()->setVisible(2, false);
	findTwoDots().lock()->setVisible(3, false);

	if (!sampler.lock()->getSound().lock())
	{
		findField("snd").lock()->setFocusable(false);
		findField("playx").lock()->setFocusable(false);
		findField("st").lock()->setFocusable(false);
		findField("end").lock()->setFocusable(false);
		findField("zone").lock()->setFocusable(false);
		ls.lock()->setFocus("dummy");
	}

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
		ls.lock()->openScreen("zonestartfine");
	}
	else if (param.compare("end") == 0)
	{
		ls.lock()->openScreen("zoneendfine");
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

		auto editSoundGui = mpc.getUis().lock()->getEditSoundGui();
		auto newSampleName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("zone");
		ls.lock()->openScreen("editsound");
		break;
	}
	case 5:
	{
		if (Mpc::instance().getControls().lock()->isF6Pressed())
		{
			return;
		}

		Mpc::instance().getControls().lock()->setF6Pressed(true);
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
	auto mtf = ls.lock()->lookupField(param).lock();
	
	if (mtf->isSplit())
	{
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
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
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));
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

void ZoneScreen::initZones(int length)
{
	if (zone > numberOfZones - 1)
	{
		zone = 0;
	}

	this->zonedSampleFrameCount = length;
	int zoneLength = (int)floor(length / numberOfZones);
	int zoneStart = 0;
	zones.clear();
	
	for (int i = 0; i < numberOfZones - 1; i++)
	{
		zones.push_back(vector<int>(2));
		zones[i][0] = zoneStart;
		zones[i][1] = zoneStart + zoneLength - 1;
		zoneStart += zoneLength;
	}
	zones.push_back(vector<int>(2));
	zones[numberOfZones - 1][0] = zoneStart;
	zones[numberOfZones - 1][1] = length;
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
	return zones[zoneIndex][0];
}

void ZoneScreen::setZoneEnd(int zoneIndex, int end)
{
	if (end < zones[zoneIndex][0]) {
		end = zones[zoneIndex][0];
	}
	if (zoneIndex < numberOfZones - 1 && end > zones[zoneIndex + 1][1]) {
		end = zones[zoneIndex + 1][1];
	}
	if (zoneIndex == numberOfZones - 1 && end > zonedSampleFrameCount) {
		end = zonedSampleFrameCount;
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
