#include <ui/sequencer/window/MetronomeSoundObserver.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Background.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

MetronomeSoundObserver::MetronomeSoundObserver(mpc::Mpc* mpc)
{
	sampler = mpc->getSampler();
	soundNames = { "CLICK", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };
	bg = mpc->getLayeredScreen().lock()->getCurrentBackground();
	swGui = mpc->getUis().lock()->getSequencerWindowGui();
	swGui->deleteObservers();
	swGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	soundField = ls->lookupField("sound");
	volumeField = ls->lookupField("volume");
	volumeLabel = ls->lookupLabel("volume");
	outputField = ls->lookupField("output");
	outputLabel = ls->lookupLabel("output");
	accentField = ls->lookupField("accent");
	accentLabel = ls->lookupLabel("accent");
	normalField = ls->lookupField("normal");
	normalLabel = ls->lookupLabel("normal");
	veloAccentField = ls->lookupField("velocityaccent");
	veloAccentLabel = ls->lookupLabel("velocityaccent");
	veloNormalField = ls->lookupField("velocitynormal");
	veloNormalLabel = ls->lookupLabel("velocitynormal");
	displaySound();
	if (swGui->getMetronomeSound() == 0) {
		displayVolume();
		displayOutput();
	}
	else {
		displayAccent();
		displayNormal();
		displayVelocityAccent();
		displayVelocityNormal();
		bg->setName("metronomesoundempty");
	}
}

void MetronomeSoundObserver::displaySound()
{
    auto sound = swGui->getMetronomeSound();
    soundField.lock()->setText(soundNames[sound]);
	/*
    volumeLabel->setVisible(sound == 0);
    volumeField->setVisible(sound == 0);
    outputLabel->setVisible(sound == 0);
    outputField->setVisible(sound == 0);
    normalLabel->setVisible(sound > 0);
    normalField->setVisible(sound > 0);
    veloNormalLabel->setVisible(sound > 0);
    veloNormalField->setVisible(sound > 0);
	accentLabel->setVisible(sound > 0);
    accentField->setVisible(sound > 0);
    veloAccentLabel->setVisible(sound > 0);
    veloAccentField->setVisible(sound > 0);
	*/
}

void MetronomeSoundObserver::displayVolume()
{
    //volumeField.lock()->setText(StringUtils::rightPad(swGui)->getClickVolume()), 3));
}

void MetronomeSoundObserver::displayOutput()
{
    outputField.lock()->setText(swGui->getClickOutput() == 0 ? "STEREO" : to_string(swGui->getClickOutput()));
}

void MetronomeSoundObserver::displayAccent()
{
	auto lSampler = sampler.lock();
	auto program = lSampler->getDrumBusProgramNumber(swGui->getMetronomeSound());
    auto pad = lSampler->getProgram(program).lock()->getPadNumberFromNote(swGui->getAccentNote());
	accentField.lock()->setText(swGui->getAccentNote() + "/" + lSampler->getPadName(pad));
}

void MetronomeSoundObserver::displayNormal()
{
	auto lSampler = sampler.lock();
	auto program = lSampler->getDrumBusProgramNumber(swGui->getMetronomeSound());
    auto pad = lSampler->getProgram(program).lock()->getPadNumberFromNote(swGui->getNormalNote());
	normalField.lock()->setText(to_string(swGui->getNormalNote()) + "/" + lSampler->getPadName(pad));
}

void MetronomeSoundObserver::displayVelocityAccent()
{
    //veloAccentField.lock()->setText(rightPad(to_string(swGui->getAccentVelo()), 3));
}

void MetronomeSoundObserver::displayVelocityNormal()
{
    //veloNormalField.lock()->setText(rightPad(to_string(swGui->getNormalVelo()), 3));
}

void MetronomeSoundObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	string s = boost::any_cast<string>(arg);
	if (s.compare("clickvolume") == 0) {
		displayVolume();
	}
	else if (s.compare("clickoutput") == 0) {
		displayOutput();
	}
	else if (s.compare("metronomesound") == 0) {
		displaySound();
		if (swGui->getMetronomeSound() == 0) {
			displayVolume();
			displayOutput();
			bg->setName("metronomesound");
		}
		else {
			displayAccent();
			displayVelocityAccent();
			displayNormal();
			displayVelocityNormal();
			bg->setName("metronomesoundempty");
		}
	}
	else if (s.compare("accentvelo") == 0) {
		displayVelocityAccent();
	}
	else if (s.compare("normalvelo") == 0) {
		displayVelocityNormal();
	}
	else if (s.compare("accentnote") == 0) {
		displayAccent();
	}
	else if (s.compare("normalnote") == 0) {
		displayNormal();
	}
}

MetronomeSoundObserver::~MetronomeSoundObserver() {
}

