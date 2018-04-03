#include <controls/sampler/PgmParamsControls.hpp>

#include <Mpc.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::controls::sampler;
using namespace std;

PgmParamsControls::PgmParamsControls(mpc::Mpc* mpc) 
	: AbstractSamplerControls(mpc)
{
}

void PgmParamsControls::function(int i)
{
	init();
		auto lSampler = sampler.lock();
	int letterNumber;
	auto sc = lSampler->getSoundCount();
	string newName;
	auto lLs = ls.lock();
	switch (i) {
	case 0:
		lLs->openScreen("programassign");
		break;
	case 1:
		lLs->openScreen("selectdrum");
		break;
	case 2:
		lLs->openScreen("drum");
		break;
	case 3:
		lLs->openScreen("purge");
		break;
	case 4:
		swGui->setAutoChromAssSnd(lSampler->getLastNp(program.lock().get())->getSndNumber(), sc);
		letterNumber = lSampler->getProgramCount() + 21;
		newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
		swGui->setNewName(newName);
		samplerGui->setPrevScreenName(csn);
		lLs->openScreen("autochromaticassignment");
		break;
	case 5:
		break;
	}
}

void PgmParamsControls::turnWheel(int i)
{
    init();
    	if (param.compare("tune") == 0) {
		lastNp->setTune(lastNp->getTune() + i);
	}
	else if (param.compare("dcymd") == 0) {
		lastNp->setDecayMode(lastNp->getDecayMode() + i);
	}
	else if(param.compare("voiceoverlap") == 0) {
        auto s = sampler.lock()->getSound(lastNp->getSndNumber()).lock();
        if(s && s->isLoopEnabled())
            return;

        lastNp->setVoiceOverlap(lastNp->getVoiceOverlap() + i);
    }
	else if (param.compare("reson") == 0) {
		lastNp->setFilterResonance(lastNp->getFilterResonance() + i);
	}
	else if (param.compare("freq") == 0) {
		lastNp->setFilterFrequency(lastNp->getFilterFrequency() + i);
	}
	else if (param.compare("decay") == 0) {
		lastNp->setDecay(lastNp->getDecay() + i);
	}
	else if (param.compare("attack") == 0) {
		lastNp->setAttack(lastNp->getAttack() + i);
	}
	else if (param.compare("pgm") == 0) {
		auto pgm = mpcSoundPlayerChannel->getProgram();
		auto candidate = sampler.lock()->getUsedProgram(pgm, i > 0);
		if (candidate != pgm) {
			mpcSoundPlayerChannel->setProgram(candidate);
		}
	}
    else if(param.compare("note") == 0) {
        auto candidate = samplerGui->getNote() + i;
        if(candidate > 34)
            samplerGui->setPadAndNote(samplerGui->getPad(), candidate);

    }
}

void PgmParamsControls::openWindow()
{
	init();
		auto lLs = ls.lock();
	int pn;
	int nn;
	if (param.compare("pgm") == 0) {
		samplerGui->setPrevScreenName(csn);
		lLs->openScreen("program");
	}
	else if (param.compare("note") == 0) {
		pn = mpcSoundPlayerChannel->getProgram();
		nn = samplerGui->getNote();
		auto pc = sampler.lock()->getProgramCount();
		swGui->setProg0(pn, pc);
		swGui->setNote0(nn);
		swGui->setProg1(pn, pc);
		swGui->setNote1(nn);
		samplerGui->setPrevScreenName(csn);
		lLs->openScreen("copynoteparameters");
	}
	else if (param.compare("attack") == 0 || param.compare("decay") == 0 || param.compare("dcymd") == 0) {
		lLs->openScreen("velocitymodulation");
	}
	else if (param.compare("freq") == 0 || param.compare("reson") == 0) {
		lLs->openScreen("veloenvfilter");
	}
	else if (param.compare("tune") == 0) {
		lLs->openScreen("velopitch");
	}
	else if (param.compare("voiceoverlap") == 0) {
		lLs->openScreen("muteassign");
	}
}
