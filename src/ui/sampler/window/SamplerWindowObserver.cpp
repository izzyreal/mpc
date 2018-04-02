#include "SamplerWindowObserver.hpp"

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/EnvGraph.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/StereoMixerChannel.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <ctootextensions/MpcSoundPlayerChannel.hpp>
#include <lang/StrUtil.hpp>

#include <cmath>

using namespace mpc::ui::sampler::window;
using namespace std;
using namespace mpc::lcdgui;

SamplerWindowObserver::SamplerWindowObserver(mpc::Mpc* mpc)
{
	letters = vector<string>{ "A" , "B", "C", "D" };
	this->mpc = mpc;
	auto uis = mpc->getUis().lock();
	samplerGui = uis->getSamplerGui();
	samplerGui->addObserver(this);
	swGui = uis->getSamplerWindowGui();
	swGui->addObserver(this);
	sampler = mpc->getSampler();
	auto ls = mpc->getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	auto lSampler = sampler.lock();
	mpcSoundPlayerChannel = lSampler->getDrum(samplerGui->getSelectedDrum());
	program = lSampler->getProgram(mpcSoundPlayerChannel->getProgram());
	auto lProgram = program.lock();
	lSampler->getLastNp(lProgram.get())->addObserver(this);
	lSampler->getLastPad(lProgram.get())->addObserver(this);
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);

	if (csn.compare("program") == 0) {
		programNameField = ls->lookupField("programname");
		midiProgramChangeField = ls->lookupField("midiprogramchange");
		displayProgramName();
		displayMidiProgramChange();
	}
	else if (csn.compare("deleteprogram") == 0) {
		pgmField = ls->lookupField("pgm");
		displayPgm();
	}
	else if (csn.compare("createnewprogram") == 0) {
		newNameField = ls->lookupField("newname");
		midiProgramChangeField = ls->lookupField("midiprogramchange");
		displayNewName();
		displayMidiProgramChange();
	}
	else if (csn.compare("copyprogram") == 0) {
		pgm0Field = ls->lookupField("pgm0");
		pgm1Field = ls->lookupField("pgm1");
		displayPgm0();
		displayPgm1();
	}
	else if (csn.compare("assignmentview") == 0) {
		info0Label = ls->lookupLabel("info0");
		info1Label = ls->lookupLabel("info1");
		info2Label = ls->lookupLabel("info2");
		a0Field = ls->lookupField("a0");
		b0Field = ls->lookupField("b0");
		c0Field = ls->lookupField("c0");
		d0Field = ls->lookupField("d0");
		a1Field = ls->lookupField("a1");
		b1Field = ls->lookupField("b1");
		c1Field = ls->lookupField("c1");
		d1Field = ls->lookupField("d1");
		a2Field = ls->lookupField("a2");
		b2Field = ls->lookupField("b2");
		c2Field = ls->lookupField("c2");
		d2Field = ls->lookupField("d2");
		a3Field = ls->lookupField("a3");
		b3Field = ls->lookupField("b3");
		c3Field = ls->lookupField("c3");
		d3Field = ls->lookupField("d3");
		info0Label.lock()->setOpaque(false);
		info1Label.lock()->setOpaque(true);
		info1Label.lock()->setInverted(true);
		info1Label.lock()->setSize(13, 9);
		auto pads = vector <weak_ptr<mpc::lcdgui::Field>> { a3Field, b3Field, c3Field, d3Field, a2Field, b2Field, c2Field, d2Field, a1Field, b1Field, c1Field, d1Field, a0Field, b0Field, c0Field, d0Field };
		ls->setFocus(swGui->getFocusFromPadNumber(samplerGui->getPad()));
		displayAssignmentView();
	}
	else if (csn.compare("keeporretry") == 0) {
		nameForNewSoundField = ls->lookupField("namefornewsound");
		assignToNoteField = ls->lookupField("assigntonote");
		displayNameForNewSound();
		displayAssignToPad();
	}
	else if (csn.compare("initpadassign") == 0) {
		initPadAssignField = ls->lookupField("initpadassign");
		displayInitPadAssign();
	}
	else if (csn.compare("copynoteparameters") == 0) {
		prog0Field = ls->lookupField("prog0");
		note0Field = ls->lookupField("note0");
		prog1Field = ls->lookupField("prog1");
		note1Field = ls->lookupField("note1");
		displayProg0();
		displayNote0();
		displayProg1();
		displayNote1();
	}
	else if (csn.compare("velocitymodulation") == 0) {
		noteField = ls->lookupField("note");
		veloAttackField = ls->lookupField("veloattack");
		veloStartField = ls->lookupField("velostart");
		veloLevelField = ls->lookupField("velolevel");
		veloField = ls->lookupField("velo");
		displayNote();
		displayVeloAttack();
		displayVeloStart();
		displayVeloLevel();
		displayVelo();
	}
	else if (csn.compare("veloenvfilter") == 0) {
		ls->getEnvGraph().lock()->Hide(false);
		noteField = ls->lookupField("note");
		attackField = ls->lookupField("attack");
		decayField = ls->lookupField("decay");
		amountField = ls->lookupField("amount");
		veloFreqField = ls->lookupField("velofreq");
		veloField = ls->lookupField("velo");
		displayNote();
		displayAttack();
		displayDecay();
		displayAmount();
		displayVeloFreq();
		displayVelo();
	}
	else if (csn.compare("velopitch") == 0) {
		noteField = ls->lookupField("note");
		tuneField = ls->lookupField("tune");
		veloPitchField = ls->lookupField("velopitch");
		veloField = ls->lookupField("velo");
		displayNote();
		displayTune();
		displayVeloPitch();
		displayVelo();
	}
	else if (csn.compare("muteassign") == 0) {
		noteField = ls->lookupField("note");
		note0Field = ls->lookupField("note0");
		note1Field = ls->lookupField("note1");
		displayNote();
		displayNote0();
		displayNote1();
	}
	else if (csn.compare("autochromaticassignment") == 0) {
		sourceField = ls->lookupField("source");
		autoChromAssSndField = ls->lookupField("snd");
		originalKeyField = ls->lookupField("originalkey");
		tuneField = ls->lookupField("tune");
		programNameField = ls->lookupField("programname");
		displaySource();
		displayAutoChromAssSnd();
		displayOriginalKey();
		displayTune();
		displayProgramName();
	}
}

void SamplerWindowObserver::displayNameForNewSound()
{
	auto lSampler = sampler.lock();
	nameForNewSoundField.lock()->setText(lSampler->getPreviewSound().lock()->getName());
}

void SamplerWindowObserver::displayAssignToPad()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	assignToNoteField.lock()->setText(to_string(lSampler->getLastPad(lProgram.get())->getNote()) + "/" + lSampler->getPadName(lSampler->getLastPad(lProgram.get())->getNumber()));
}

void SamplerWindowObserver::displaySource()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto nn = lSampler->getLastNp(lProgram.get())->getNumber();
	auto pn = lProgram->getPadNumberFromNote(nn);
	string nnName = to_string(nn);
	auto padName = pn == -1 ? "OFF" : lSampler->getPadName(pn);
	sourceField.lock()->setText(nnName + "/" + padName);
}

void SamplerWindowObserver::displayTune()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	if (csn.compare("autochromaticassignment") == 0) {
		int value = swGui->getTune();
		auto prefix = value < 0 ? "-" : " ";
		tuneField.lock()->setText(prefix + moduru::lang::StrUtil::padLeft(to_string(abs(value)), " ", 3));
	}
	else {
		auto value = lSampler->getLastNp(lProgram.get())->getTune();
		auto prefix = value < 0 ? "-" : " ";
		tuneField.lock()->setText(prefix + moduru::lang::StrUtil::padLeft(to_string(abs(value)), " ", 3));
	}
}

void SamplerWindowObserver::displayVeloPitch()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto value = lSampler->getLastNp(lProgram.get())->getVelocityToPitch();
	auto prefix = value < 0 ? "-" : " ";
	veloPitchField.lock()->setText(prefix + moduru::lang::StrUtil::padLeft(to_string(abs(value)), " ", 3));
}

void SamplerWindowObserver::displayAttack()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto attack = lSampler->getLastNp(lProgram.get())->getFilterAttack();
	auto decay = lSampler->getLastNp(lProgram.get())->getFilterDecay();
	attackField.lock()->setTextPadded(attack, " ");
	mpc->getLayeredScreen().lock()->redrawEnvGraph(attack, decay);
}

void SamplerWindowObserver::displayDecay()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto attack = lSampler->getLastNp(lProgram.get())->getFilterAttack();
	auto decay = lSampler->getLastNp(lProgram.get())->getFilterDecay();
	decayField.lock()->setTextPadded(decay, " ");
	mpc->getLayeredScreen().lock()->redrawEnvGraph(attack, decay);
}

void SamplerWindowObserver::displayAmount()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	amountField.lock()->setTextPadded(lSampler->getLastNp(lProgram.get())->getFilterEnvelopeAmount(), " ");
}

void SamplerWindowObserver::displayVeloFreq()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	veloFreqField.lock()->setTextPadded(lSampler->getLastNp(lProgram.get())->getVelocityToFilterFrequency(), " ");
}

void SamplerWindowObserver::displayNote()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto noteParameters = lSampler->getLastNp(lProgram.get());
	auto sn = noteParameters->getSndNumber();
	auto pn = lProgram->getPadNumberFromNote(noteParameters->getNumber());
	auto pad = lProgram->getPad(pn);
	auto padName = pn != -1 ? lSampler->getPadName(pn) : "OFF";
	auto sampleName = sn != -1 ? lSampler->getSoundName(sn) : "OFF";
	auto stereo = pad->getStereoMixerChannel().lock()->isStereo() && sn != -1 ? "(ST)" : "";
	noteField.lock()->setText(to_string(noteParameters->getNumber()) + "/" + padName + "-" + moduru::lang::StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void SamplerWindowObserver::displayVeloAttack()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	veloAttackField.lock()->setTextPadded(lSampler->getLastNp(lProgram.get())->getVelocityToAttack(), " ");
}

void SamplerWindowObserver::displayVeloStart()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	veloStartField.lock()->setTextPadded(lSampler->getLastNp(lProgram.get())->getVelocityToStart(), " ");
}

void SamplerWindowObserver::displayVeloLevel()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	veloLevelField.lock()->setTextPadded(lSampler->getLastNp(lProgram.get())->getVeloToLevel(), " ");
}

void SamplerWindowObserver::displayVelo()
{
}

void SamplerWindowObserver::displayProg0()
{
	auto lSampler = sampler.lock();
	prog0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getProg0() + 1), " ", 2) + "-" + lSampler->getProgram(swGui->getProg0()).lock()->getName());
}

void SamplerWindowObserver::displayNote0()
{
	auto lSampler = sampler.lock();
	auto prog = csn.compare("muteassign") == 0 ? program.lock() : lSampler->getProgram(swGui->getProg0()).lock();
	auto nn = csn.compare("muteassign") == 0 ? lSampler->getLastNp(prog.get())->getMuteAssignA() : swGui->getNote0();
	auto pn = prog->getPadNumberFromNote(nn);
	auto sn = nn != 34 ? prog->getNoteParameters(nn)->getSndNumber() : -1;
	auto nnName = nn == 34 ? "--" : to_string(nn);
	auto padName = pn != -1 ? lSampler->getPadName(pn) : "OFF";
	auto sampleName = sn != -1 ? "-" + lSampler->getSoundName(sn) : "-OFF";
	if (nn == -1) sampleName = "";
	note0Field.lock()->setText(nnName + "/" + padName + sampleName);
}

void SamplerWindowObserver::displayProg1()
{
	auto lSampler = sampler.lock();
	prog1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getProg1() + 1), " ", 2) + "-" + lSampler->getProgram(swGui->getProg1()).lock()->getName());
}

void SamplerWindowObserver::displayNote1()
{
	auto lSampler = sampler.lock();
	auto prog = csn.compare("muteassign") == 0 ? program.lock() : lSampler->getProgram(swGui->getProg1()).lock();
	auto nn = csn.compare("muteassign") == 0 ? lSampler->getLastNp(prog.get())->getMuteAssignB() : swGui->getNote1();
	auto pn = prog->getPadNumberFromNote(nn);
	auto sn = nn != 34 ? prog->getNoteParameters(nn)->getSndNumber() : -1;
	auto nnName = nn == 34 ? "--" : to_string(nn);
	auto padName = pn != -1 ? lSampler->getPadName(pn) : "OFF";
	auto sampleName = sn != -1 ? "-" + lSampler->getSoundName(sn) : "-OFF";
	if (nn == 34) sampleName = "";
	note1Field.lock()->setText(nnName + "/" + padName + sampleName);
}

void SamplerWindowObserver::displayInitPadAssign()
{
	initPadAssignField.lock()->setText(swGui->isInitPadAssignMaster() ? "MASTER" : "PROGRAM");
}

void SamplerWindowObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();

	lSampler->getLastNp(lProgram.get())->deleteObserver(this);
	lSampler->getLastPad(lProgram.get())->deleteObserver(this);
	lProgram->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);

	mpcSoundPlayerChannel = lSampler->getDrum(samplerGui->getSelectedDrum());
	program = lSampler->getProgram(mpcSoundPlayerChannel->getProgram());

	lSampler->getLastNp(lProgram.get())->addObserver(this);
	lSampler->getLastPad(lProgram.get())->addObserver(this);
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);

	string s = boost::any_cast<string>(arg);

	if (s.compare("newprogramchange") == 0 || s.compare("midiprogramchange") == 0) {
		displayMidiProgramChange();
	}
	else if (s.compare("deletepgm") == 0) {
		displayPgm();
	}
	else if (s.compare("pgm0") == 0) {
		displayPgm0();
	}
	else if (s.compare("pgm1") == 0) {
		displayPgm1();
	}
	else if (s.compare("bank") == 0) {
		displayAssignmentView();
	}
	else if (s.compare("padandnote") == 0) {
		if (csn.compare("keeporretry") == 0) {
			displayAssignToPad();
		}
		else if (csn.compare("assignmentview") == 0) {
			mpc->getLayeredScreen().lock()->setFocus(swGui->getFocusFromPadNumber(samplerGui->getPad()));
			displayAssignmentView();
		}
		else if (csn.compare("velocitymodulation") == 0) {
			displayNote();
			displayVeloAttack();
			displayVeloStart();
			displayVeloLevel();
		}
		else if (csn.compare("veloenvfilter") == 0) {
			displayNote();
			displayAttack();
			displayDecay();
			displayAmount();
			displayVeloFreq();
		}
		else if (csn.compare("velopitch") == 0) {
			displayNote();
			displayTune();
			displayVeloPitch();
		}
		else if (csn.compare("muteassign") == 0) {
			displayNote();
			displayNote0();
			displayNote1();
		}
		else if (csn.compare("autochromaticassignment") == 0) {
			displaySource();
		}

	}
	else if (s.compare("note") == 0) {
		if (csn.compare("assignmentview") == 0) {
			auto pn = swGui->getPadNumberFromFocus(mpc->getLayeredScreen().lock()->getFocus(), samplerGui->getBank());
			displayInfo1();
			displayInfo2();
			displayPad(pn);
		}
	}
	else if (s.compare("initpadassign") == 0) {
		displayInitPadAssign();
	}
	else if (s.compare("prog0") == 0) {
		displayProg0();
		displayNote0();
	}
	else if (s.compare("muteassigna") == 0 || s.compare("note0") == 0) {
		displayNote0();
	}
	else if (s.compare("prog1") == 0) {
		displayProg1();
		displayNote1();
	}
	else if (s.compare("muteassignb") == 0 || s.compare("note1") == 0) {
		displayNote1();
	}
	else if (s.compare("velocitytoattack") == 0) {
		displayVeloAttack();
	}
	else if (s.compare("velocitytostart") == 0) {
		displayVeloStart();
	}
	else if (s.compare("velocitytolevel") == 0) {
		displayVeloLevel();
	}
	else if (s.compare("filterattack") == 0) {
		displayAttack();
	}
	else if (s.compare("filterdecay") == 0) {
		displayDecay();
	}
	else if (s.compare("filterenvelopeamount") == 0) {
		displayAmount();
	}
	else if (s.compare("velocitytofilterfrequency") == 0) {
		displayVeloFreq();
	}
	else if (s.compare("tune") == 0) {
		displayTune();
	}
	else if (s.compare("velocitytopitch") == 0) {
		displayVeloPitch();
	}
	else if (s.compare("autochromasssnd") == 0) {
		displayAutoChromAssSnd();
	}
	else if (s.compare("originalkey") == 0) {
		displayOriginalKey();
	}
	else if (s.compare("namefornewsound") == 0) {
		displayNameForNewSound();
	}
}

void SamplerWindowObserver::displayOriginalKey()
{
	auto lSampler = sampler.lock();
	auto nn = swGui->getOriginalKey();
	auto lProgram = program.lock();
	auto pad = lProgram->getPadNumberFromNote(nn);
	auto pn = lSampler->getPadName(pad);
	originalKeyField.lock()->setText(to_string(nn) + "/" + pn);
}

void SamplerWindowObserver::displayAutoChromAssSnd()
{
	auto lSampler = sampler.lock();
	auto sn = swGui->getAutoChromAssSnd();
	auto sampleName = sn == -1 ? "OFF" : lSampler->getSoundName(sn);
	auto stereo = sn == -1 ? "" : lSampler->getSound(sn).lock()->isMono() ? "" : "(ST)";
	autoChromAssSndField.lock()->setText(moduru::lang::StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void SamplerWindowObserver::displayAssignmentView()
{
	for (int i = 0; i < 16; i++)
		displayPad(i);

	displayInfo0();
	displayInfo1();
	displayInfo2();
}

void SamplerWindowObserver::displayInfo0()
{
	info0Label.lock()->setText("Bank:" + letters[samplerGui->getBank()] + " Note:");
}

void SamplerWindowObserver::displayInfo1()
{
	auto focus = mpc->getLayeredScreen().lock()->getFocus();
	auto pn = swGui->getPadNumberFromFocus(focus, samplerGui->getBank());
	int nn = program.lock()->getPad(pn)->getNote();
	info1Label.lock()->setText(nn != 34 ? to_string(nn) : "--");
}

void SamplerWindowObserver::displayInfo2()
{
	auto focus = mpc->getLayeredScreen().lock()->getFocus();
	auto pn = swGui->getPadNumberFromFocus(focus, samplerGui->getBank());
	int nn = program.lock()->getPad(pn)->getNote();

	if (nn == 34) {
		info2Label.lock()->setText("=");
		return;
	}

	int sampleNumber = program.lock()->getNoteParameters(nn)->getSndNumber();

	string sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "";

	string stereo = "";
	auto lastPad = sampler.lock()->getLastPad(program.lock().get());
	if (sampleNumber != -1 && lastPad->getStereoMixerChannel().lock()->isStereo() && sampleName.compare("") != 0) {
		stereo = "(ST)";
	}
	info2Label.lock()->setText("=" + sampleName + stereo);
}

void SamplerWindowObserver::displayPad(int i)
{
	auto lSampler = sampler.lock();
	auto pads = vector<weak_ptr<Field>>{ a3Field, b3Field, c3Field, d3Field, a2Field, b2Field, c2Field, d2Field, a1Field, b1Field, c1Field, d1Field, a0Field, b0Field, c0Field, d0Field };
	auto lProgram = program.lock();
	auto nn = lProgram->getPad(i + (16 * samplerGui->getBank()))->getNote();
	string sampleName = "";
	if (nn != 34) {
		auto sampleNumber = lProgram->getNoteParameters(nn)->getSndNumber();
		sampleName = sampleNumber != -1 ? lSampler->getSoundName(sampleNumber) : "--";
		if (sampleName.length() > 8) {
			sampleName = sampleName.substr(0, 8);
		}
		while (!sampleName.empty() && isspace(sampleName.back())) sampleName.pop_back();
	}
	pads[i].lock()->setText(sampleName);
}

void SamplerWindowObserver::displayPgm0()
{
	auto lSampler = sampler.lock();
	pgm0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getPgm0() + 1), " ", 2) + "-" + lSampler->getProgram(swGui->getPgm0()).lock()->getName());
}

void SamplerWindowObserver::displayPgm1()
{
	auto lSampler = sampler.lock();
	pgm1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getPgm1() + 1), " ", 2) + "-" + lSampler->getProgram(swGui->getPgm1()).lock()->getName());
}

void SamplerWindowObserver::displayNewName()
{
	newNameField.lock()->setText(swGui->getNewName());
}

void SamplerWindowObserver::displayPgm()
{
	auto lSampler = sampler.lock();
	pgmField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getDeletePgm() + 1), " ", 2) + "-" + lSampler->getProgram(swGui->getDeletePgm()).lock()->getName());
}

void SamplerWindowObserver::displayProgramName()
{
	if (csn.compare("autochromaticassignment") == 0) {
		programNameField.lock()->setText(swGui->getNewName());
	}
	else {
		auto lProgram = program.lock();
		programNameField.lock()->setText(lProgram->getName());
	}
}

void SamplerWindowObserver::displayMidiProgramChange()
{
	auto lProgram = program.lock();
	midiProgramChangeField.lock()->setTextPadded(lProgram->getMidiProgramChange(), " ");
}

SamplerWindowObserver::~SamplerWindowObserver() {
	if (mpc->getLayeredScreen().lock()) {
		mpc->getLayeredScreen().lock()->getEnvGraph().lock()->Hide(true);
	}
	samplerGui->deleteObserver(this);
	swGui->deleteObserver(this);
	program.lock()->deleteObserver(this);
	sampler.lock()->getLastNp(program.lock().get())->deleteObserver(this);
	sampler.lock()->getLastPad(program.lock().get())->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
}
