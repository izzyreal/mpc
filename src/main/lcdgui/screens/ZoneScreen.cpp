#include "ZoneScreen.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <sampler/Sampler.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
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
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		soundGui->setPreviousNumberOfZones(soundGui->getNumberOfZones());
		ls.lock()->openScreen("numberofzones");
	}
	else if (param.compare("st") == 0)
	{
		ls.lock()->openScreen("zonestartfine");
	}
	else if (param.compare("end") == 0) {
		ls.lock()->openScreen("zoneendfine");
	}
}

void ZoneScreen::function(int f)
{
	init();

	auto soundGui = mpc.getUis().lock()->getSoundGui();

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
		auto zone = vector<int>{ soundGui->getZoneStart(soundGui->getZoneNumber()) , soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		sampler.lock()->playX(soundGui->getPlayX(), &zone);
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
	
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	auto zone = soundGui->getZoneNumber();
	
	if (param.compare("st") == 0)
	{
		soundGui->setZoneStart(zone, soundGui->getZoneStart(zone) + soundInc);
		displaySt();
		displayWave();
	}
	else if (param.compare("end") == 0)
	{
		soundGui->setZoneEnd(zone, soundGui->getZoneEnd(zone) + soundInc);
		displayEnd();
		displayWave();
	}
	else if (param.compare("zone") == 0)
	{
		soundGui->setZone(soundGui->getZoneNumber() + i);
		displayZone();
		displaySt();
		displayEnd();
		displayWave();
	}
	else if (param.compare("playx") == 0)
	{
		soundGui->setPlayX(soundGui->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->setSoundGuiNextSound();
		displayEnd();
		displaySnd();
		displaySt();
		displayWave();
		displayZone();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->setSoundGuiPrevSound();
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
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findWave().lock()->setSampleData(sampleData, sampler.lock()->getSound().lock()->isMono(), soundGui->getView());
	findWave().lock()->setSelection(soundGui->getZoneStart(soundGui->getZoneNumber()), soundGui->getZoneEnd(soundGui->getZoneNumber()));
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
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("playx").lock()->setText(playXNames[soundGui->getPlayX()]);
}

void ZoneScreen::displaySt()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		findField("st").lock()->setTextPadded(soundGui->getZoneStart(soundGui->getZoneNumber()), " ");
	}
	else {
		findField("st").lock()->setText("       0");
	}
}

void ZoneScreen::displayEnd()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		findField("end").lock()->setTextPadded(soundGui->getZoneEnd(soundGui->getZoneNumber()), " ");
	}
	else {
		findField("end").lock()->setText("       0");
	}
}

void ZoneScreen::displayZone()
{
	if (sampler.lock()->getSoundCount() == 0) {
		findField("zone").lock()->setText("1");
		return;
	}
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("zone").lock()->setText(to_string(soundGui->getZoneNumber() + 1));
}
