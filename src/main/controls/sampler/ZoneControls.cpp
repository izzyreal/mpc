#include <controls/sampler/ZoneControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler;
using namespace std;

ZoneControls::ZoneControls() 
	: AbstractSamplerControls()
{
}

void ZoneControls::init()
{
	super::init();
	this->typableParams = vector<string>{ "st", "end" };
}

void ZoneControls::openWindow()
{
	init();
	auto lLs = ls.lock();
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(soundGui->getSoundIndex(), sampler.lock()->getSoundCount());
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

void ZoneControls::function(int f)
{
	init();
		string newSampleName;
	auto lLs = ls.lock();
	vector<int> zone;
	auto lSampler = sampler.lock();
	switch (f) {
	case 0:
		lLs->openScreen("trim");
		break;
	case 1:
		lLs->openScreen("loop");
		break;
	case 2:
		lSampler->sort();
		break;
	case 3:
		lLs->openScreen("params");
		break;
	case 4:
		if (lSampler->getSoundCount() == 0)
			return;

		newSampleName = lSampler->getSoundName(soundGui->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = lSampler->addOrIncreaseNumber(newSampleName);
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
		lSampler->playX(soundGui->getPlayX(), &zone);
		break;
	}
}

void ZoneControls::turnWheel(int i)
{
	init();
	if (param == "") return;
	auto lSampler = sampler.lock();
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
		lSampler->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0) {
		lSampler->setSoundGuiPrevSound();
	}
}
