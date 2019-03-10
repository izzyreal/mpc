#include <ui/sampler/DrumObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sampler;
using namespace std;

DrumObserver::DrumObserver(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	samplerGui = mpc->getUis().lock()->getSamplerGui();
	sampler = mpc->getSampler();
	mpcSoundPlayerChannel = sampler.lock()->getDrum(samplerGui->getSelectedDrum());
	mpcSoundPlayerChannel->addObserver(this);
	samplerGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	drumField = ls->lookupField("drum");
	padToInternalSoundField = ls->lookupField("padtointernalsound");
	pgmField = ls->lookupField("pgm");
	pgmChangeField = ls->lookupField("programchange");
	midiVolumeField = ls->lookupField("midivolume");
	currentValField = ls->lookupField("currentval");
	displayDrum();
	displayPadToInternalSound();
	displayPgm();
	displayPgmChange();
	displayMidiVolume();
	displayCurrentVal();
}

void DrumObserver::displayValue()
{
    valueLabel.lock()->setTextPadded(sampler.lock()->getUnusedSampleAmount(), " ");
}

void DrumObserver::displayDrum()
{
    drumField.lock()->setText(to_string(samplerGui->getSelectedDrum() + 1));
}

void DrumObserver::displayPadToInternalSound()
{
    padToInternalSoundField.lock()->setText(samplerGui->isPadToIntSound() ? "ON" : "OFF");
}

void DrumObserver::displayPgm()
{
    auto pn = mpcSoundPlayerChannel->getProgram();
    pgmField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(pn + 1), " ", 2) + "-" +  dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pn).lock())->getName());
}

void DrumObserver::displayPgmChange()
{
    pgmChangeField.lock()->setText(mpcSoundPlayerChannel->receivesPgmChange() ? "RECEIVE" : "IGNORE");
}

void DrumObserver::displayMidiVolume()
{
    midiVolumeField.lock()->setText(mpcSoundPlayerChannel->receivesMidiVolume() ? "RECEIVE" : "IGNORE");
}

void DrumObserver::displayCurrentVal()
{
}

void DrumObserver::update(moduru::observer::Observable* o, std::any arg)
{
	mpcSoundPlayerChannel->deleteObserver(this);
	mpcSoundPlayerChannel = sampler.lock()->getDrum(samplerGui->getSelectedDrum());
	mpcSoundPlayerChannel->addObserver(this);

	string s = std::any_cast<string>(arg);

	if (s.compare("drum") == 0) {
		displayDrum();
		displayPgm();
		displayPgmChange();
		displayMidiVolume();
		displayCurrentVal();
	}
	else if (s.compare("pgm") == 0) {
		displayPgm();
	}
	else if (s.compare("padtointsound") == 0) {
		displayPadToInternalSound();
	}
	else if (s.compare("receivepgmchange") == 0) {
		displayPgmChange();
	}
	else if (s.compare("receivemidivolume") == 0) {
		displayMidiVolume();
	}
}

DrumObserver::~DrumObserver() {
	mpcSoundPlayerChannel->deleteObserver(this);
	samplerGui->deleteObserver(this);
}
