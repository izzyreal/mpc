#include <ui/sampler/SoundObserver.hpp>

#include <Util.hpp>
#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Wave.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sampler;
using namespace std;

SoundObserver::SoundObserver()
{
	sampler = Mpc::instance().getSampler();
	
	qualityNames = vector<string>{ "LOW", "MED", "HIGH" };
	bitNames = vector<string>{ "16", "12", "8" };
	auto ls = Mpc::instance().getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	soundGui = Mpc::instance().getUis().lock()->getSoundGui();
	soundGui->addObserver(this);

	if (csn.compare("resample") == 0) {
		newFsField = ls->lookupField("newfs");
		qualityField = ls->lookupField("quality");
		newBitField = ls->lookupField("newbit");
		newNameField = ls->lookupField("newname");
		displayNewFs();
		displayQuality();
		displayNewBit();
		displayNewName();
	}
	else if (csn.compare("stereotomono") == 0) {
		stereoSourceField = ls->lookupField("stereosource");
		newLNameField = ls->lookupField("newlname");
		newRNameField = ls->lookupField("newrname");
		displayStereoSource();
		displayNewLName();
		displayNewRName();
	}
	else if (csn.compare("monotostereo") == 0) {
		lSourceField = ls->lookupField("lsource");
		rSourceField = ls->lookupField("rsource");
		newStNameField = ls->lookupField("newstname");
		displayLSource();
		displayRSource();
		displayNewStName();
	}
}

void SoundObserver::displayLSource()
{
	auto lSampler = sampler.lock();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	lSourceField.lock()->setText(lSampler->getSound().lock()->getName());
	if (lSampler->getSound().lock()->isMono() && lSampler->getSound(soundGui->getRSource()).lock()->isMono()) {
		//ls->setFunctionKeysArrangement("monotostereo");
		//ls->getWave().lock()->SetDirty();
		//ls->getLayer(0)->setDirty();
		//ls->getLayer(1)->setDirty();
		//ls->getLayer(2)->setDirty();
	}
	else {
		ls->setFunctionKeysArrangement(1);
		//ls->getWave().lock()->SetDirty();
		//ls->getLayer(0)->setDirty();
		//ls->getLayer(1)->setDirty();
		//ls->getLayer(2)->setDirty();
	}
}

void SoundObserver::displayRSource()
{
	auto lSampler = sampler.lock();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	rSourceField.lock()->setText(lSampler->getSoundName(soundGui->getRSource()));
	if (lSampler->getSound().lock()->isMono() && lSampler->getSound(soundGui->getRSource()).lock()->isMono()) {
		//ls->setFunctionKeysArrangement("monotostereo");
		//ls->getWave().lock()->SetDirty();
		//ls->getLayer(0)->setDirty();
		//ls->getLayer(1)->setDirty();
		//ls->getLayer(2)->setDirty();
	}
	else {
		ls->setFunctionKeysArrangement(1);
		//ls->getWave().lock()->SetDirty();
		//ls->getLayer(0)->setDirty();
		//ls->getLayer(1)->setDirty();
		//ls->getLayer(2)->setDirty();
	}
}

void SoundObserver::displayNewStName()
{
    newStNameField.lock()->setText(soundGui->getNewStName());
}

void SoundObserver::displayStereoSource()
{
	auto lSampler = sampler.lock();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	stereoSourceField.lock()->setText(lSampler->getSound().lock()->getName());
	if (lSampler->getSound().lock()->isMono()) {
		ls->setFunctionKeysArrangement(1);
		//ls->getWave().lock()->SetDirty();
		//ls->getLayer(0)->setDirty();
		//ls->getLayer(1)->setDirty();
		//ls->getLayer(2)->setDirty();
	}
	else {
		//ls->setFunctionKeysArrangement("stereotomono");
		//ls->getWave().lock()->SetDirty();
		//ls->getLayer(0)->setDirty();
		//ls->getLayer(1)->setDirty();
		//ls->getLayer(2)->setDirty();
	}
}

void SoundObserver::displayNewLName()
{
    newLNameField.lock()->setText(soundGui->getNewLName());
}

void SoundObserver::displayNewRName()
{
    newRNameField.lock()->setText(soundGui->getNewRName());
}

void SoundObserver::displayNewFs()
{
    newFsField.lock()->setText(to_string(soundGui->getNewFs()));
}

void SoundObserver::displayQuality()
{
    qualityField.lock()->setText(qualityNames[soundGui->getQuality()]);
}

void SoundObserver::displayNewBit()
{
    newBitField.lock()->setText(bitNames[soundGui->getNewBit()]);
}

void SoundObserver::displayNewName()
{
    newNameField.lock()->setText(soundGui->getNewName());
}

void SoundObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("soundindex") == 0) {
		if (csn.compare("stereotomono") == 0) {
			displayStereoSource();
		}
		else if (csn.compare("monotostereo") == 0) {
			displayLSource();
		}
	}
	else if (s.compare("newfs") == 0) {
		displayNewFs();
	}
	else if (s.compare("newbit") == 0) {
		displayNewBit();
	}
	else if (s.compare("quality") == 0) {
		displayQuality();
	}
	else if (s.compare("newname") == 0) {
		displayNewName();
	}
	else if (s.compare("stereosource") == 0) {
		displayStereoSource();
	}
	else if (s.compare("rsource") == 0) {
		if (csn.compare("monotostereo") == 0) {
			displayRSource();
		}
	}
}

SoundObserver::~SoundObserver() {
	soundGui->deleteObserver(this);
}
