#include "ZoneScreen.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

ZoneScreen::ZoneScreen(const int layerIndex) 
	: ScreenComponent("zone", layerIndex)
{
}

void ZoneScreen::init()
{
	super::init();
	this->typableParams = vector<string>{ "st", "end" };
}

void ZoneScreen::openWindow()
{
	init();
	auto lLs = ls.lock();
	if (param.compare("snd") == 0) {
		setSoundIndex(soundGui->getSoundIndex(), sampler.lock()->getSoundCount());
		soundGui->setPreviousScreenName("zone");
		lLs->openScreen("sound");
	}
	else if (param.compare("zone") == 0) {
		soundGui->setPreviousNumberOfZones(soundGui->getNumberOfZones());
		lLs->openScreen("numberofzones");
	}
	else if (param.compare("st") == 0) {
		lLs->openScreen("zonestartfine");
	}
	else if (param.compare("end") == 0) {
		lLs->openScreen("zoneendfine");
	}
}

void ZoneScreen::function(int f)
{
	init();
		string newSampleName;
	auto lLs = ls.lock();
	vector<int> zone;
	
	switch (f) {
	case 0:
		lLs->openScreen("trim");
		break;
	case 1:
		lLs->openScreen("loop");
		break;
	case 2:
		sampler.lock()->sort();
		break;
	case 3:
		lLs->openScreen("params");
		break;
	case 4:
		if (sampler.lock()->getSoundCount() == 0)
			return;

		newSampleName = sampler.lock()->getSoundName(soundGui->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("zone");
		lLs->openScreen("editsound");
		break;
	case 5:
		if (Mpc::instance().getControls().lock()->isF6Pressed()) {
			return;
		}
		
		Mpc::instance().getControls().lock()->setF6Pressed(true);
		
		zone = vector<int>{ soundGui->getZoneStart(soundGui->getZoneNumber()) , soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		sampler.lock()->playX(soundGui->getPlayX(), &zone);
		break;
	}
}

void ZoneScreen::turnWheel(int i)
{
	init();
	if (param == "") return;
	
		auto soundInc = getSoundIncrement(i);
	auto mtf = ls.lock()->lookupField(param).lock();
	if (mtf->isSplit()) {
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}
	auto zone = soundGui->getZoneNumber();
	if (param.compare("st") == 0) {
		soundGui->setZoneStart(zone, soundGui->getZoneStart(zone) + soundInc);
	}
	else if (param.compare("end") == 0) {
		soundGui->setZoneEnd(zone, soundGui->getZoneEnd(zone) + soundInc);
	}
	else if (param.compare("zone") == 0) {
		soundGui->setZone(soundGui->getZoneNumber() + i);
	}
	else if (param.compare("playx") == 0) {
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("snd") == 0 && i > 0) {
		sampler.lock()->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0) {
		sampler.lock()->setSoundGuiPrevSound();
	}
}
