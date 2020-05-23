#include <lcdgui/screens/PgmAssignScreen.hpp>

#include <Mpc.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

PgmAssignScreen::PgmAssignScreen(const int layerIndex)
	: ScreenComponent("programassign", layerIndex)
{
}

void PgmAssignScreen::function(int i)
{
	init();
		
	auto lLs = ls.lock();
	int letterNumber;
	string newName;
	switch (i) {
	case 0:
		lLs->openScreen("selectdrum");
		break;
	case 1:
		lLs->openScreen("programparams");
		break;
	case 2:
		lLs->openScreen("drum");
		break;
	case 3:
		lLs->openScreen("purge");
		break;
	case 4:
		swGui->setAutoChromAssSnd(sampler.lock()->getLastNp(program.lock().get())->getSndNumber(), sampler.lock()->getSoundCount());
		letterNumber = sampler.lock()->getProgramCount() + 21;
		newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
		swGui->setNewName(newName);
		samplerGui->setPrevScreenName(csn);
		lLs->openScreen("autochromaticassignment");
		break;
	}
}

void PgmAssignScreen::turnWheel(int i)
{
	init();
	if (param.compare("padassign") == 0) {
		samplerGui->setPadAssignMaster(i > 0);
	}
	else if (param.compare("pgm") == 0) {
		auto pgm = mpcSoundPlayerChannel->getProgram();
		auto candidate = sampler.lock()->getUsedProgram(pgm, i > 0);
		if (candidate != pgm) {
			mpcSoundPlayerChannel->setProgram(candidate);
		}
	}
	else if (param.compare("pad") == 0) {
		auto candidate = samplerGui->getPad() + i;
		
		if (candidate < 0 || candidate > 63)
			return;

		auto nextNN = program.lock()->getPad(candidate)->getNote();
		samplerGui->setPadAndNote(candidate, nextNN);
	}
	else if (param.compare("padnote") == 0) {
		lastPad->setNote(lastPad->getNote() + i);
		auto candidate = lastPad->getNote();
		if (candidate > 34)
			samplerGui->setPadAndNote(samplerGui->getPad(), candidate);

	}
	else if (param.compare("note") == 0) {
		auto candidate = samplerGui->getNote() + i;
		if (candidate > 34)
			samplerGui->setPadAndNote(samplerGui->getPad(), candidate);

	}
	else if (param.compare("snd") == 0) {
		
		lastNp->setSoundNumber(sampler.lock()->getNextSoundIndex(lastNp->getSndNumber(), i > 0));
		auto sound = sampler.lock()->getSound(lastNp->getSndNumber()).lock();
		if (sound) {
			if (sound->isMono()) {
				lastPad->getStereoMixerChannel().lock()->setStereo(false);
			}
			if (sound->isLoopEnabled()) {
				lastNp->setVoiceOverlap(2);
			}
		}
	}
	else if (param.compare("mode") == 0) {
		lastNp->setSoundGenMode(lastNp->getSoundGenerationMode() + i);
	}
	else if (param.compare("velocityrangelower") == 0) {
		lastNp->setVeloRangeLower(lastNp->getVelocityRangeLower() + i);
	}
	else if (param.compare("velocityrangeupper") == 0) {
		lastNp->setVeloRangeUpper(lastNp->getVelocityRangeUpper() + i);
	}
	else if (param.compare("optionalnotenumbera") == 0) {
		lastNp->setOptNoteA(lastNp->getOptionalNoteA() + i);
	}
	else if (param.compare("optionalnotenumberb") == 0) {
		lastNp->setOptionalNoteB(lastNp->getOptionalNoteB() + i);
	}
}

void PgmAssignScreen::openWindow()
{
	init();
		auto lLs = ls.lock();
	
	if (param.compare("pgm") == 0) {
		samplerGui->setPrevScreenName("programassign");
		lLs->openScreen("program");
	}
	else if (param.compare("pad") == 0 || param.compare("padnote") == 0) {
		lLs->openScreen("assignmentview");
	}
	else if (param.compare("padassign") == 0) {
		lLs->openScreen("initpadassign");
	}
	else if (param.compare("note") == 0) {
		auto pn = mpcSoundPlayerChannel->getProgram();
		auto nn = samplerGui->getNote();
		auto pc = sampler.lock()->getProgramCount();
		swGui->setProg0(pn, pc);
		swGui->setNote0(nn);
		swGui->setProg1(pn, pc);
		swGui->setNote1(nn);
		lLs->openScreen("copynoteparameters");
	}
	else if (param.compare("snd") == 0) {
		auto sn = sampler.lock()->getLastNp(program.lock().get())->getSndNumber();
		if (sn != -1) {
			soundGui->setSoundIndex(sn, sampler.lock()->getSoundCount());
			soundGui->setPreviousScreenName("programassign");
			lLs->openScreen("sound");
		}
	}
}
