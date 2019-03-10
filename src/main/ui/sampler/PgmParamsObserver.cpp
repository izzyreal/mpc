#include <ui/sampler/PgmParamsObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/EnvGraph.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sampler;
using namespace std;

PgmParamsObserver::PgmParamsObserver(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	decayModes = { "END", "START" };
	voiceOverlapModes = { "POLY", "MONO", "NOTE OFF" };
	samplerGui = mpc->getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);
	sampler = mpc->getSampler();
	ls = mpc->getLayeredScreen().lock().get();
	auto lSampler = sampler.lock();
	mpcSoundPlayerChannel = lSampler->getDrum(samplerGui->getSelectedDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	auto lProgram = program.lock();
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);
	lastNp = lSampler->getLastNp(lProgram.get());
	lastNp->addObserver(this);
	pgmField = ls->lookupField("pgm");
	noteField = ls->lookupField("note");
	attackField = ls->lookupField("attack");
	decayField = ls->lookupField("decay");
	decayModeField = ls->lookupField("dcymd");
	freqField = ls->lookupField("freq");
	resonField = ls->lookupField("reson");
	tuneField = ls->lookupField("tune");
	voiceOverlapField = ls->lookupField("voiceoverlap");
	displayPgm();
	displayNote();
	displayDecayMode();
	displayFreq();
	displayReson();
	displayTune();
	displayVoiceOverlap();
	displayAttackDecay();
	ls->getEnvGraph().lock()->Hide(false);
}

void PgmParamsObserver::update(moduru::observer::Observable* o, std::any arg)
{
	string s = std::any_cast<string>(arg);
	auto lProgram = program.lock();
	auto lSampler = sampler.lock();
	lProgram->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
	lastNp->deleteObserver(this);

	mpcSoundPlayerChannel = lSampler->getDrum(samplerGui->getSelectedDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	lProgram = program.lock();

	lastNp = lSampler->getLastNp(lProgram.get());
	lastNp->addObserver(this);
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);

	if (s.compare("pgm") == 0 || s.compare("padandnote") == 0) {
		displayPgm();
		displayNote();
		displayAttackDecay();
		displayDecayMode();
		displayFreq();
		displayReson();
		displayTune();
		displayVoiceOverlap();
	}
	else if (s.compare("attack") == 0 || s.compare("decay") == 0) {
		displayAttackDecay();
	}
	else if (s.compare("dcymd") == 0) {
		displayDecayMode();
	}
	else if (s.compare("freq") == 0) {
		displayFreq();
	}
	else if (s.compare("reson") == 0) {
		displayReson();
	}
	else if (s.compare("tune") == 0) {
		displayTune();
	}
	else if (s.compare("voiceoverlap") == 0) {
		displayVoiceOverlap();
	}
	else if (s.compare("note") == 0) {
		displayNote();
	}
}

void PgmParamsObserver::displayReson()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	resonField.lock()->setText(to_string(lSampler->getLastNp(lProgram.get())->getFilterResonance()));
}

void PgmParamsObserver::displayFreq()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	freqField.lock()->setText(to_string(lSampler->getLastNp(lProgram.get())->getFilterFrequency()));
}

void PgmParamsObserver::displayAttackDecay()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto attack = lSampler->getLastNp(lProgram.get())->getAttack();
	auto decay = lSampler->getLastNp(lProgram.get())->getDecay();
	attackField.lock()->setTextPadded(attack, " ");
	decayField.lock()->setTextPadded(decay, " ");
	ls->redrawEnvGraph(attack, decay);
}

void PgmParamsObserver::displayNote()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto sampleNumber = lSampler->getLastNp(lProgram.get())->getSndNumber();
	auto note = lSampler->getLastNp(lProgram.get())->getNumber();
	auto sampleName = sampleNumber != -1 ? lSampler->getSoundName(sampleNumber) : "OFF";
	auto padNumber = lProgram->getPadNumberFromNote(note);
	if (padNumber != -1) {
		auto stereo = lProgram->getPad(padNumber)->getStereoMixerChannel().lock()->isStereo() && sampleNumber != -1 ? "(ST)" : "";
		auto padName = lSampler->getPadName(padNumber);
		noteField.lock()->setText(to_string(note) + "/" + padName + "-" + moduru::lang::StrUtil::padRight(sampleName, " ", 16) + stereo);
	}
	else {
		noteField.lock()->setText(to_string(note) + "/OFF-" + sampleName);
	}
}

void PgmParamsObserver::displayPgm()
{
	pgmField.lock()->setTextPadded(mpcSoundPlayerChannel->getProgram() + 1, " ");
}

void PgmParamsObserver::displayTune()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	tuneField.lock()->setText(to_string(lSampler->getLastNp(lProgram.get())->getTune()));
}

void PgmParamsObserver::displayDecayMode()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	decayModeField.lock()->setText(decayModes[lSampler->getLastNp(lProgram.get())->getDecayMode()]);
}

void PgmParamsObserver::displayVoiceOverlap()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	voiceOverlapField.lock()->setText(voiceOverlapModes[lSampler->getLastNp(lProgram.get())->getVoiceOverlap()]);
}

PgmParamsObserver::~PgmParamsObserver() {
	samplerGui->deleteObserver(this);
	program.lock()->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
	lastNp->deleteObserver(this);

	if (mpc->getLayeredScreen().lock()) {
		mpc->getLayeredScreen().lock()->getEnvGraph().lock()->Hide(true);
	}
}
