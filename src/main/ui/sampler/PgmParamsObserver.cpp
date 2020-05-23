#include <ui/sampler/PgmParamsObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/EnvGraph.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>

#include <ui/sampler/SamplerGui.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::ui::sampler;
using namespace std;

PgmParamsObserver::PgmParamsObserver() 
{
	
	decayModes = { "END", "START" };
	voiceOverlapModes = { "POLY", "MONO", "NOTE OFF" };
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);
	sampler = Mpc::instance().getSampler();
	ls = Mpc::instance().getLayeredScreen().lock().get();

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));

	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->getDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	auto lProgram = program.lock();
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);
	lastNp = sampler.lock()->getLastNp(lProgram.get());
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

void PgmParamsObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	auto lProgram = program.lock();
	lProgram->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
	lastNp->deleteObserver(this);

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->getDrum());

	program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	lProgram = program.lock();

	lastNp = sampler.lock()->getLastNp(lProgram.get());
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
	auto lProgram = program.lock();
	resonField.lock()->setText(to_string(sampler.lock()->getLastNp(lProgram.get())->getFilterResonance()));
}

void PgmParamsObserver::displayFreq()
{
	auto lProgram = program.lock();
	freqField.lock()->setText(to_string(sampler.lock()->getLastNp(lProgram.get())->getFilterFrequency()));
}

void PgmParamsObserver::displayAttackDecay()
{
	auto lProgram = program.lock();
	auto attack = sampler.lock()->getLastNp(lProgram.get())->getAttack();
	auto decay = sampler.lock()->getLastNp(lProgram.get())->getDecay();
	attackField.lock()->setTextPadded(attack, " ");
	decayField.lock()->setTextPadded(decay, " ");
	ls->redrawEnvGraph(attack, decay);
}

void PgmParamsObserver::displayNote()
{
	auto lProgram = program.lock();
	auto sampleNumber = sampler.lock()->getLastNp(lProgram.get())->getSndNumber();
	auto note = sampler.lock()->getLastNp(lProgram.get())->getNumber();
	auto sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "OFF";
	auto padNumber = lProgram->getPadNumberFromNote(note);
	if (padNumber != -1) {
		auto stereo = lProgram->getPad(padNumber)->getStereoMixerChannel().lock()->isStereo() && sampleNumber != -1 ? "(ST)" : "";
		auto padName = sampler.lock()->getPadName(padNumber);
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
	auto lProgram = program.lock();
	tuneField.lock()->setText(to_string(sampler.lock()->getLastNp(lProgram.get())->getTune()));
}

void PgmParamsObserver::displayDecayMode()
{
	auto lProgram = program.lock();
	decayModeField.lock()->setText(decayModes[sampler.lock()->getLastNp(lProgram.get())->getDecayMode()]);
}

void PgmParamsObserver::displayVoiceOverlap()
{
	auto lProgram = program.lock();
	voiceOverlapField.lock()->setText(voiceOverlapModes[sampler.lock()->getLastNp(lProgram.get())->getVoiceOverlap()]);
}

PgmParamsObserver::~PgmParamsObserver() {
	samplerGui->deleteObserver(this);
	program.lock()->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
	lastNp->deleteObserver(this);

	if (Mpc::instance().getLayeredScreen().lock()) {
		Mpc::instance().getLayeredScreen().lock()->getEnvGraph().lock()->Hide(true);
	}
}
