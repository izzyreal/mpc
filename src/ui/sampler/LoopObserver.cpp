#include "LoopObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/TwoDots.hpp>
#include <lcdgui/Wave.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <ctootextensions/MpcSoundOscillatorControls.hpp>

using namespace mpc::ui::sampler;
using namespace std;

LoopObserver::LoopObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	this->sampler = mpc->getSampler();
	playXNames = vector<string>{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
	soundGui = mpc->getUis().lock()->getSoundGui();
	soundGui->addObserver(this);
	auto lSampler = sampler.lock();
	if (lSampler->getSoundCount() != 0) {
		sound = lSampler->getSound(soundGui->getSoundIndex());
		auto lSound = sound.lock();
		lSound->addObserver(this);
		lSound->getMsoc()->addObserver(this);
	}
	auto ls = mpc->getLayeredScreen().lock();
	twoDots = ls->getTwoDots();
	twoDots.lock()->Hide(false);
	twoDots.lock()->setVisible(0, true);
	twoDots.lock()->setVisible(1, true);
	twoDots.lock()->setVisible(2, false);
	twoDots.lock()->setVisible(3, false);
	wave = ls->getWave();
	wave.lock()->Hide(false);
	sndField = ls->lookupField("snd");
	playXField = ls->lookupField("playx");
	toField = ls->lookupField("to");
	endLengthField = ls->lookupField("endlength");
	endLengthValueField = ls->lookupField("endlengthvalue");
	loopField = ls->lookupField("loop");
	dummyField = ls->lookupField("dummy");
	toField.lock()->setSize(9 * 6 + 1, 9);
	endLengthValueField.lock()->setSize(9 * 6 + 1, 9);
	displaySnd();
	displayPlayX();
	displayTo();
	displayEndLength();
	displayEndLengthValue();
	displayLoop();
	if (lSampler->getSoundCount() != 0) {
		dummyField.lock()->setFocusable(false);
		waveformLoadData();
		auto lSound = sound.lock();
		wave.lock()->setSelection(lSound->getLoopTo(), lSound->getEnd());
		soundGui->initZones(sampler.lock()->getSound(soundGui->getSoundIndex()).lock()->getLastFrameIndex());
	}
	else {
		wave.lock()->setSampleData(nullptr, false, 0);
		sndField.lock()->setFocusable(false);
		playXField.lock()->setFocusable(false);
		toField.lock()->setFocusable(false);
		endLengthField.lock()->setFocusable(false);
		endLengthValueField.lock()->setFocusable(false);
		loopField.lock()->setFocusable(false);
	}
}

void LoopObserver::displaySnd()
{
	auto lSampler = sampler.lock();
	auto ls = mpc->getLayeredScreen().lock();
	if (lSampler->getSoundCount() != 0) {
		if (ls->getFocus().compare("dummy") == 0) ls->setFocus(sndField.lock()->getName());
		auto lSound = sound.lock();
		lSound->deleteObserver(this);
		lSound->getMsoc()->deleteObserver(this);
		sound = lSampler->getSound(soundGui->getSoundIndex());
		lSound = sound.lock();
		lSound->addObserver(this);
		lSound->getMsoc()->addObserver(this);
		auto sampleName = lSound->getName();
		if (!lSound->isMono()) {
			sampleName = moduru::lang::StrUtil::padRight(sampleName, " ", 16) + "(ST)";
		}
		sndField.lock()->setText(sampleName);
	}
	else {
		sndField.lock()->setText("(no sound)");
		ls->setFocus("dummy");
	}
}

void LoopObserver::displayPlayX()
{
    playXField.lock()->setText(playXNames[soundGui->getPlayX()]);
}

void LoopObserver::displayTo()
{
	if (sampler.lock()->getSoundCount() != 0) {
		toField.lock()->setTextPadded(sound.lock()->getLoopTo(), " ");
	}
	else {
		toField.lock()->setTextPadded("0", " ");
	}
	if (!soundGui->isEndSelected()) displayEndLengthValue();
}

void LoopObserver::displayEndLength()
{
	endLengthField.lock()->setSize(31, 9);
	endLengthField.lock()->setText(soundGui->isEndSelected() ? "  End" : "Lngth");
	displayEndLengthValue();
	mpc->getLayeredScreen().lock()->lookupLabel("endlengthvalue").lock()->SetDirty();
}

void LoopObserver::displayEndLengthValue()
{
	if (sampler.lock()->getSoundCount() != 0) {
		if (soundGui->isEndSelected()) {
			endLengthValueField.lock()->setTextPadded(sound.lock()->getEnd(), " ");
		}
		else {
			auto lSound = sound.lock();
			endLengthValueField.lock()->setTextPadded(lSound->getEnd() - lSound->getLoopTo(), " ");
		}
	}
	else {
		endLengthValueField.lock()->setTextPadded("0", " ");
	}
}

void LoopObserver::displayLoop()
{
	if (sampler.lock()->getSoundCount() == 0) {
		loopField.lock()->setText("OFF");
		return;
	}
	loopField.lock()->setText(sound.lock()->isLoopEnabled() ? "ON" : "OFF");
}

void LoopObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	string s = boost::any_cast<string>(arg);
	if (s.compare("soundnumber") == 0) {
		auto lSound = sound.lock();
		displaySnd();
		displayTo();
		displayEndLength();
		waveformLoadData();
		wave.lock()->setSelection(lSound->getLoopTo(), lSound->getEnd());
		soundGui->initZones(lSound->getLastFrameIndex());
	}
	else if (s.compare("loopto") == 0) {
		displayTo();
		auto lSound = sound.lock();
		wave.lock()->setSelection(lSound->getLoopTo(), lSound->getEnd());
	}
	else if (s.compare("endlength") == 0) {
		displayEndLength();
	}
	else if (s.compare("end") == 0) {
		displayEndLengthValue();
		auto lSound = sound.lock();
		wave.lock()->setSelection(lSound->getLoopTo(), lSound->getEnd());
	}
	else if (s.compare("loopenabled") == 0) {
		displayLoop();
	}
	else if (s.compare("playx") == 0) {
		displayPlayX();
	}
}

void LoopObserver::waveformLoadData()
{
	auto lSound = sound.lock();
	auto sampleData = lSound->getSampleData();
	wave.lock()->setSampleData(sampleData, lSound->isMono(), soundGui->getView());
}

LoopObserver::~LoopObserver() {
	if (wave.lock()) {
		//wave.lock()->Hide(true);
		//wave.lock()->setSampleData(nullptr, false, 0);
	}
	if (twoDots.lock()) {
		twoDots.lock()->Hide(true);
	}
	soundGui->deleteObserver(this);
	auto lSound = sound.lock();
	if (lSound) {
		lSound->deleteObserver(this);
		lSound->getMsoc()->deleteObserver(this);
	}
}
