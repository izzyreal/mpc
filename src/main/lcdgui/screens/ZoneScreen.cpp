#include "ZoneScreen.hpp"

#include <lcdgui/Layer.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/window/NumberOfZonesScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::controls;
using namespace moduru::lang;

ZoneScreen::ZoneScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "zone", layerIndex)
{
	addChildT<Wave>()->setFine(false);
}

void ZoneScreen::open()
{
    mpc.getControls().lock()->getControls()->typableParams = { "st", "end" };

    if (zones.empty())
		initZones();

	bool sound = sampler->getSound().lock() ? true : false;
	findField("snd")->setFocusable(sound);
	findField("playx")->setFocusable(sound);
	findField("st")->setFocusable(sound);
	findField("st")->enableTwoDots();
	findField("end")->setFocusable(sound);
	findField("end")->enableTwoDots();
	findField("zone")->setFocusable(sound);
	findField("dummy")->setFocusable(!sound);

	displayWave();
	displaySnd();
	displayPlayX();
	displaySt();
	displayEnd();
	displayZone();

	ls->setFunctionKeysArrangement(sound ? 1 : 0);
}

void ZoneScreen::openWindow()
{
	init();

	if (param.compare("snd") == 0)
	{
		sampler->setPreviousScreenName("zone");
		openScreen("sound");
	}
	else if (param.compare("zone") == 0)
	{
		openScreen("number-of-zones");
	}
	else if (param.compare("st") == 0)
	{
		openScreen("zone-start-fine");
	}
	else if (param.compare("end") == 0)
	{
		openScreen("zone-end-fine");
	}
}

void ZoneScreen::function(int f)
{
	init();

	switch (f)
	{
	case 0:
		openScreen("trim");
		break;
	case 1:
		openScreen("loop");
		break;
	case 2:
	{
		sampler->sort();
		openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Sorting by " + sampler->getSoundSortingTypeName());
		popupScreen->returnToScreenAfterMilliSeconds("zone", 200);
		break;
	}
	case 3:
		openScreen("params");
		break;
	case 4:
	{
		if (sampler->getSoundCount() == 0)
			return;

		auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
		editSoundScreen->setReturnToScreenName("zone");
		openScreen("edit-sound");
		break;
	}
	case 5:
	{
		if (mpc.getControls().lock()->isF6Pressed())
			return;

		mpc.getControls().lock()->setF6Pressed(true);
		sampler->playX();
		break;
	}
	}
}

void ZoneScreen::left()
{
	splitLeft();
}

void ZoneScreen::right()
{
	splitRight();
}

void ZoneScreen::turnWheel(int i)
{
	init();

	auto sound = sampler->getSound().lock();

	if (param == "" || !sound)
		return;

	auto soundInc = getSoundIncrement(i);
	auto field = findField(param);

	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

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
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler->selectNextSound();
		displayEnd();
		displaySnd();
		displaySt();
		displayWave();
		displayZone();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler->selectPreviousSound();
		displayEnd();
		displaySnd();
		displaySt();
		displayWave();
		displayZone();
	}
}

void ZoneScreen::displayWave()
{
	auto sound = sampler->getSound().lock();

	if (!sound)
	{
		findWave()->setSampleData(nullptr, true, 0);
		findWave()->setSelection(0, 0);
		return;
	}

	auto sampleData = sound->getSampleData();
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	findWave()->setSampleData(sampleData, sampler->getSound().lock()->isMono(), trimScreen->view);
	findWave()->setSelection(getZoneStart(zone), getZoneEnd(zone));
}

void ZoneScreen::displaySnd()
{
	auto sound = sampler->getSound().lock();

	if (!sound)
	{
		findField("snd")->setText("(no sound)");
		ls->setFocus("dummy");
		return;
	}

	if (ls->getFocus().compare("dummy") == 0)
		ls->setFocus("snd");

	auto sampleName = sound->getName();

	if (!sound->isMono())
		sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";

	findField("snd")->setText(sampleName);
}

void ZoneScreen::displayPlayX()
{
	findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void ZoneScreen::displaySt()
{
	if (sampler->getSoundCount() != 0)
	{
		findField("st")->setTextPadded(getZoneStart(zone), " ");
	}
	else {
		findField("st")->setText("       0");
	}
}

void ZoneScreen::displayEnd()
{
	if (sampler->getSoundCount() != 0)
	{
		findField("end")->setTextPadded(getZoneEnd(zone), " ");
	}
	else {
		findField("end")->setText("       0");
	}
}

void ZoneScreen::displayZone()
{
	if (sampler->getSoundCount() == 0)
	{
		findField("zone")->setTextPadded(1);
		return;
	}

	findField("zone")->setTextPadded(zone + 1);
}

void ZoneScreen::initZones()
{
	zones.clear();
	
	auto sound = sampler->getSound().lock();

	if (!sound)
	{
		zone = 0;
		return;
	}

	float zoneLength = sound->getFrameCount() / float(numberOfZones);
	float zoneStart = 0.f;

	for (int i = 0; i < numberOfZones - 1; i++)
	{
		zones.push_back(std::vector<int>(2));
		zones[i][0] = (int) floor(zoneStart);
		zones[i][1] = (int) floor(zoneStart + zoneLength);
		zoneStart += zoneLength;
	}

	zones.push_back(std::vector<int>(2));
	zones[numberOfZones - 1][0] = (int)floor(zoneStart);
	zones[numberOfZones - 1][1] = sound->getFrameCount();
	zone = 0;
}

void ZoneScreen::setZoneStart(int zoneIndex, int start)
{
	if (start > zones[zoneIndex][1])
		start = zones[zoneIndex][1];
	
	if (zoneIndex == 0 && start < 0)
		start = 0;
	
	if (zoneIndex > 0 && start < zones[zoneIndex - 1][0])
		start = zones[zoneIndex - 1][0];
	
	zones[zoneIndex][0] = start;
	
	if (zoneIndex != 0)
		zones[zoneIndex - 1][1] = start;
	
	displaySt();
	displayWave();
}

int ZoneScreen::getZoneStart(int zoneIndex)
{
	if (zoneIndex >= zones.size())
		return 0;

	return zones[zoneIndex][0];
}

void ZoneScreen::setZoneEnd(int zoneIndex, int end)
{
	auto length = sampler->getSound().lock()->getFrameCount();

	if (end < zones[zoneIndex][0])
		end = zones[zoneIndex][0];

	if (zoneIndex < numberOfZones - 1 && end > zones[zoneIndex + 1][1])
		end = zones[zoneIndex + 1][1];

	if (zoneIndex == numberOfZones - 1 && end > length)
		end = length;

	zones[zoneIndex][1] = end;

	if (zoneIndex != numberOfZones - 1)
		zones[zoneIndex + 1][0] = end;
	
	displayEnd();
	displayWave();
}

int ZoneScreen::getZoneEnd(int zoneIndex)
{
	if (zoneIndex >= zones.size())
		return 0;

	return zones[zoneIndex][1];
}

void ZoneScreen::setZone(int i)
{
	if (i < 0 || i >= numberOfZones)
		return;

	zone = i;

	displayWave();
	displaySt();
	displayEnd();
	displayZone();
}

std::vector<int> ZoneScreen::getZone()
{
	return std::vector<int>{ getZoneStart(zone), getZoneEnd(zone) };
}

void ZoneScreen::pressEnter()
{
	if (mpc.getControls().lock()->isShiftPressed())
	{
		openScreen("save");
		return;
	}

	init();

	auto field = ls->getFocusedLayer()->findField(param);

	if (!field->isTypeModeEnabled())
		return;

	auto candidate = field->enter();
	auto sound = sampler->getSound().lock();

	if (candidate != INT_MAX)
	{
		if (param.compare("st") == 0 || param.compare("start") == 0)
		{
			auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
			zoneScreen->setZoneStart(zoneScreen->zone, candidate);
			displaySt();
			displayWave();
		}
		else if (param.compare("end") == 0)
		{
			auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
			zoneScreen->setZoneEnd(zoneScreen->zone, candidate);
			displayEnd();
			displayWave();
		}
	}

}
