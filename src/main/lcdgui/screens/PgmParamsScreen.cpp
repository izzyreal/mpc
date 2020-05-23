#include "PgmParamsScreen.hpp"

#include <Mpc.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

PgmParamsScreen::PgmParamsScreen(const int layerIndex) 
	: ScreenComponent("programparams", layerIndex)
{
}

void PgmParamsScreen::function(int i)
{
	init();
		
	auto sc = sampler.lock()->getSoundCount();

	switch (i)
	{
	case 0:
		ls.lock()->openScreen("programassign");
		break;
	case 1:
		ls.lock()->openScreen("selectdrum");
		break;
	case 2:
		ls.lock()->openScreen("drum");
		break;
	case 3:
		ls.lock()->openScreen("purge");
		break;
	case 4:
	{
		auto samplerWindowGui = mpc.getUis().lock()->getSamplerWindowGui();
		samplerWindowGui->setAutoChromAssSnd(sampler.lock()->getLastNp(program.lock().get())->getSndNumber(), sc);
		auto letterNumber = sampler.lock()->getProgramCount() + 21;
		auto newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
		samplerWindowGui->setNewName(newName);
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("autochromaticassignment");
		break;
	}
	case 5:
		break;
	}
}

void PgmParamsScreen::turnWheel(int i)
{
    init();
    
	auto lastPad = sampler.lock()->getLastPad(program.lock().get());
	auto lastNoteParameters = dynamic_cast<mpc::sampler::NoteParameters*>(program.lock()->getNoteParameters(lastPad->getNote()));
	
	if (param.compare("tune") == 0)
	{
		lastNoteParameters->setTune(lastNoteParameters->getTune() + i);
	}
	else if (param.compare("dcymd") == 0) {
		lastNoteParameters->setDecayMode(lastNoteParameters->getDecayMode() + i);
	}
	else if(param.compare("voiceoverlap") == 0) {
        auto s = sampler.lock()->getSound(lastNoteParameters->getSndNumber()).lock();
		
		if (s && s->isLoopEnabled())
		{
			return;
		}
        
		lastNoteParameters->setVoiceOverlap(lastNoteParameters->getVoiceOverlap() + i);
    }
	else if (param.compare("reson") == 0)
	{
		lastNoteParameters->setFilterResonance(lastNoteParameters->getFilterResonance() + i);
	}
	else if (param.compare("freq") == 0)
	{
		lastNoteParameters->setFilterFrequency(lastNoteParameters->getFilterFrequency() + i);
	}
	else if (param.compare("decay") == 0)
	{
		lastNoteParameters->setDecay(lastNoteParameters->getDecay() + i);
	}
	else if (param.compare("attack") == 0)
	{
		lastNoteParameters->setAttack(lastNoteParameters->getAttack() + i);
	}
	else if (param.compare("pgm") == 0)
	{	
		auto pgm = mpcSoundPlayerChannel->getProgram();
		auto candidate = sampler.lock()->getUsedProgram(pgm, i > 0);
		
		if (candidate != pgm)
		{
			mpcSoundPlayerChannel->setProgram(candidate);
		}
	}
    else if(param.compare("note") == 0)
	{
        auto candidate = samplerGui->getNote() + i;
		if (candidate > 34)
		{
			samplerGui->setPadAndNote(samplerGui->getPad(), candidate);
		}
    }
}

void PgmParamsScreen::openWindow()
{
	init();

	if (param.compare("pgm") == 0) {
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("program");
	}
	else if (param.compare("note") == 0) {
		auto programIndex = mpcSoundPlayerChannel->getProgram();
		
		auto note = samplerGui->getNote();
		auto programCount = sampler.lock()->getProgramCount();
		auto samplerWindowGui = mpc.getUis().lock()->getSamplerWindowGui();
		samplerWindowGui->setProg0(programIndex, programCount);
		samplerWindowGui->setNote0(note);
		samplerWindowGui->setProg1(programIndex, programCount);
		samplerWindowGui->setNote1(note);
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("copynoteparameters");
	}
	else if (param.compare("attack") == 0 || param.compare("decay") == 0 || param.compare("dcymd") == 0) {
		ls.lock()->openScreen("velocitymodulation");
	}
	else if (param.compare("freq") == 0 || param.compare("reson") == 0) {
		ls.lock()->openScreen("veloenvfilter");
	}
	else if (param.compare("tune") == 0) {
		ls.lock()->openScreen("velopitch");
	}
	else if (param.compare("voiceoverlap") == 0) {
		ls.lock()->openScreen("muteassign");
	}
}
