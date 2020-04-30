#include <ui/sampler/SndParamsObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <mpc/MpcSoundOscillatorControls.hpp>

#include <lang/StrUtil.hpp>
#include <cmath>

using namespace mpc::ui::sampler;
using namespace std;

SndParamsObserver::SndParamsObserver()
{
	
	sampler = Mpc::instance().getSampler();
	
	playXNames = vector<string>{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
	soundGui = Mpc::instance().getUis().lock()->getSoundGui();
	soundGui->addObserver(this);
	auto lSampler = sampler.lock();
	if (lSampler->getSoundCount() != 0) {
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
		auto lSound = sound.lock();
		lSound->addObserver(this);
		lSound->getOscillatorControls()->addObserver(this);
		soundGui->initZones(sampler.lock()->getSound(soundGui->getSoundIndex()).lock()->getLastFrameIndex());
	}
	auto ls = Mpc::instance().getLayeredScreen().lock();
	sndField = ls->lookupField("snd");
	playXField = ls->lookupField("playx");
	levelField = ls->lookupField("level");
	tuneField = ls->lookupField("tune");
	beatField = ls->lookupField("beat");
	sampleTempoLabel = ls->lookupLabel("sampletempo");
	newTempoLabel = ls->lookupLabel("newtempo");
	dummyField = ls->lookupField("dummy");
	if (lSampler->getSoundCount() != 0) {
		dummyField.lock()->setFocusable(false);
	}
	displaySnd();
	displayPlayX();
	displayLevel();
	displayTune();
	displayBeat();
	displaySampleAndNewTempo();
}

void SndParamsObserver::displayLevel()
{
	auto lSound = sound.lock();
	if (lSound) {
		levelField.lock()->setText(to_string(lSound->getSndLevel()));
	}
	else {
		levelField.lock()->setText("100");
	}
}

void SndParamsObserver::displayTune()
{
	auto lSound = sound.lock();
	if (lSound) {
		tuneField.lock()->setText(to_string(lSound->getTune()));
	}
	else {
		tuneField.lock()->setText("0");
	}
}

void SndParamsObserver::displayBeat()
{
	auto lSound = sound.lock();
	if (lSound) {
		beatField.lock()->setText(to_string(lSound->getBeatCount()));
	}
	else {
		beatField.lock()->setText("4");
	}
}

void SndParamsObserver::displaySampleAndNewTempo()
{
	auto lSound = sound.lock();
	if (!lSound) {
		sampleTempoLabel.lock()->setText("");
		newTempoLabel.lock()->setText("");
		return;
	}
	auto length = lSound->getEnd() - lSound->getLoopTo();
	auto lengthMs = (float)(length / (lSound->getSampleRate() / 1000.0));
	auto bpm = (int)(600000.0 / (lengthMs / lSound->getBeatCount()));
	auto bpmString = to_string(bpm);
	auto part1 = bpmString.substr(0, bpmString.length() - 1);
	auto part2 = bpmString.substr(bpmString.length() - 1);

	if (bpm < 300 || bpm > 3000) {
		part1 = "---";
		part2 = "-";
	}
	bpmString = part1 + "." + part2;
	bpmString = moduru::lang::StrUtil::padLeft(bpmString, " ", 5);

	bpmString = Util::replaceDotWithSmallSpaceDot(bpmString);

	sampleTempoLabel.lock()->setText("Sample tempo=" + bpmString);

	auto newBpm = (int)(pow(2.0, (lSound->getTune() / 120.0)) * bpm);
	bpmString = to_string(newBpm);
	part1 = bpmString.substr(0, bpmString.length() - 1);
	part2 = bpmString.substr(bpmString.length() - 1);
	if (newBpm < 300 || newBpm > 3000) {
		part1 = "---";
		part2 = "-";
	}

	bpmString = part1 + "." + part2;
	bpmString = moduru::lang::StrUtil::padLeft(bpmString, " ", 5);

	bpmString = Util::replaceDotWithSmallSpaceDot(bpmString);
	
	newTempoLabel.lock()->setText("New tempo=" + bpmString);
}

void SndParamsObserver::displaySnd()
{
	auto lSound = sound.lock();
	if (sampler.lock()->getSoundCount() != 0) {
		auto ls = Mpc::instance().getLayeredScreen().lock();
		if (ls->getFocus().compare("dummy") == 0) ls->setFocus(sndField.lock()->getName());
		auto sampleName = lSound->getName();
		if (!lSound->isMono()) {
			sampleName = moduru::lang::StrUtil::padRight(sampleName, " ", 16);
			sampleName += "(ST)";
		}
		sndField.lock()->setText(sampleName);
	}
	else {
        sndField.lock()->setText("(no sound)");
 //       dummyField->grabFocus();
    }
}

void SndParamsObserver::displayPlayX()
{
	playXField.lock()->setText(playXNames[soundGui->getPlayX()]);
}

void SndParamsObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto lSampler = sampler.lock();
	
	if (lSampler->getSoundCount() != 0) {
		auto lSound = sound.lock();
		lSound->deleteObserver(this);
		lSound->getOscillatorControls()->deleteObserver(this);
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
		lSound = sound.lock();
		lSound->addObserver(this);
		lSound->getOscillatorControls()->addObserver(this);
	}

	string s = nonstd::any_cast<string>(arg);

	if (s.compare("soundnumber") == 0) {
		auto lSound = sound.lock();
		displaySnd();
		soundGui->initZones(lSound->getLastFrameIndex());
		displayBeat();
		displaySampleAndNewTempo();
		displayTune();
		displayLevel();
	}
	else if (s.compare("playx") == 0) {
		displayPlayX();
	}
	else if (s.compare("beat") == 0) {
		displayBeat();
		displaySampleAndNewTempo();
	}
	else if (s.compare("tune") == 0) {
		displayTune();
		displaySampleAndNewTempo();
	}
	else if (s.compare("level") == 0) {
		displayLevel();
	}
}

SndParamsObserver::~SndParamsObserver() {
	soundGui->deleteObserver(this);
	auto lSampler = sampler.lock();
	if (lSampler->getSoundCount() != 0) {
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
		auto lSound = sound.lock();
		lSound->deleteObserver(this);
		lSound->getOscillatorControls()->deleteObserver(this);
	}
}
