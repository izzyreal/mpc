#include "PgmParamsScreen.hpp"

#include <Mpc.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/EnvGraph.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

PgmParamsScreen::PgmParamsScreen(const int layerIndex) 
	: ScreenComponent("programparams", layerIndex)
{
}

void PgmParamsScreen::open()
{
	mpc.addObserver(this);
	addChild(make_shared<EnvGraph>());
	displayPgm();
	displayNote();
	displayDecayMode();
	displayFreq();
	displayReson();
	displayTune();
	displayVoiceOverlap();
	displayAttackDecay();
}

void PgmParamsScreen::close()
{
	mpc.deleteObserver(this);
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
		ls.lock()->openScreen("select-drum");
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
		mpc.setPreviousSamplerScreenName(currentScreenName);
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
		displayTune();
	}
	else if (param.compare("dcymd") == 0)
	{
		lastNoteParameters->setDecayMode(lastNoteParameters->getDecayMode() + i);
		displayDecayMode();
	}
	else if(param.compare("voiceoverlap") == 0)
	{
        auto s = sampler.lock()->getSound(lastNoteParameters->getSndNumber()).lock();
		
		if (s && s->isLoopEnabled())
		{
			return;
		}
        
		lastNoteParameters->setVoiceOverlap(lastNoteParameters->getVoiceOverlap() + i);
		displayVoiceOverlap();
    }
	else if (param.compare("reson") == 0)
	{
		lastNoteParameters->setFilterResonance(lastNoteParameters->getFilterResonance() + i);
		displayReson();
	}
	else if (param.compare("freq") == 0)
	{
		lastNoteParameters->setFilterFrequency(lastNoteParameters->getFilterFrequency() + i);
		displayFreq();
	}
	else if (param.compare("decay") == 0)
	{
		lastNoteParameters->setDecay(lastNoteParameters->getDecay() + i);
		displayAttackDecay();
	}
	else if (param.compare("attack") == 0)
	{
		lastNoteParameters->setAttack(lastNoteParameters->getAttack() + i);
		displayAttackDecay();
	}
	else if (param.compare("pgm") == 0)
	{	
		auto pgm = mpcSoundPlayerChannel->getProgram();
		auto candidate = sampler.lock()->getUsedProgram(pgm, i > 0);
		
		if (candidate != pgm)
		{
			mpcSoundPlayerChannel->setProgram(candidate);
			displayPgm();
			displayAttackDecay();
			displayDecayMode();
			displayFreq();
			displayNote();
			displayReson();
			displayTune();
			displayVoiceOverlap();
		}
	}
    else if(param.compare("note") == 0)
	{
        auto candidate = mpc.getNote() + i;
		if (candidate > 34)
		{
			mpc.setPadAndNote(mpc.getPad(), candidate);
			displayAttackDecay();
			displayDecayMode();
			displayFreq();
			displayNote();
			displayReson();
			displayTune();
			displayVoiceOverlap();
		}
    }
}

void PgmParamsScreen::openWindow()
{
	init();

	if (param.compare("pgm") == 0)
	{
		mpc.setPreviousSamplerScreenName(currentScreenName);
		ls.lock()->openScreen("program");
	}
	else if (param.compare("note") == 0)
	{
		auto programIndex = mpcSoundPlayerChannel->getProgram();
		
		auto note = mpc.getNote();
		auto programCount = sampler.lock()->getProgramCount();
		auto samplerWindowGui = mpc.getUis().lock()->getSamplerWindowGui();
		samplerWindowGui->setProg0(programIndex, programCount);
		samplerWindowGui->setNote0(note);
		samplerWindowGui->setProg1(programIndex, programCount);
		samplerWindowGui->setNote1(note);
		mpc.setPreviousSamplerScreenName(currentScreenName);
		ls.lock()->openScreen("copynoteparameters");
	}
	else if (param.compare("attack") == 0 || param.compare("decay") == 0 || param.compare("dcymd") == 0)
	{
		ls.lock()->openScreen("velocitymodulation");
	}
	else if (param.compare("freq") == 0 || param.compare("reson") == 0)
	{
		ls.lock()->openScreen("veloenvfilter");
	}
	else if (param.compare("tune") == 0)
	{
		ls.lock()->openScreen("velopitch");
	}
	else if (param.compare("voiceoverlap") == 0)
	{
		ls.lock()->openScreen("muteassign");
	}
}

void PgmParamsScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("padandnote") == 0)
	{
		displayAttackDecay();
		displayDecayMode();
		displayFreq();
		displayNote();
		displayReson();
		displayTune();
		displayVoiceOverlap();
	}
}

void PgmParamsScreen::displayReson()
{
	init();
	auto lProgram = program.lock();
	findField("reson").lock()->setText(to_string(sampler.lock()->getLastNp(lProgram.get())->getFilterResonance()));
}

void PgmParamsScreen::displayFreq()
{
	init();
	auto lProgram = program.lock();
	findField("freq").lock()->setText(to_string(sampler.lock()->getLastNp(lProgram.get())->getFilterFrequency()));
}

void PgmParamsScreen::displayAttackDecay()
{
	init();
	auto lProgram = program.lock();
	auto attack = sampler.lock()->getLastNp(lProgram.get())->getAttack();
	auto decay = sampler.lock()->getLastNp(lProgram.get())->getDecay();
	findField("attack").lock()->setTextPadded(attack, " ");
	findField("decay").lock()->setTextPadded(decay, " ");
	findEnvGraph().lock()->setCoordinates(attack, decay);
}

void PgmParamsScreen::displayNote()
{
	init();
	auto lProgram = program.lock();
	auto sampleNumber = sampler.lock()->getLastNp(lProgram.get())->getSndNumber();
	auto note = sampler.lock()->getLastNp(lProgram.get())->getNumber();
	auto sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "OFF";
	auto padNumber = lProgram->getPadIndexFromNote(note);
	
	if (padNumber != -1)
	{
		auto stereo = lProgram->getPad(padNumber)->getStereoMixerChannel().lock()->isStereo() && sampleNumber != -1 ? "(ST)" : "";
		auto padName = sampler.lock()->getPadName(padNumber);
		findField("note").lock()->setText(to_string(note) + "/" + padName + "-" + moduru::lang::StrUtil::padRight(sampleName, " ", 16) + stereo);
	}
	else
	{
		findField("note").lock()->setText(to_string(note) + "/OFF-" + sampleName);
	}
}

void PgmParamsScreen::displayPgm()
{
	init();
	findField("pgm").lock()->setTextPadded(mpcSoundPlayerChannel->getProgram() + 1, " ");
}

void PgmParamsScreen::displayTune()
{
	init();
	auto lProgram = program.lock();
	findField("tune").lock()->setText(to_string(sampler.lock()->getLastNp(lProgram.get())->getTune()));
}

void PgmParamsScreen::displayDecayMode()
{
	init();
	auto lProgram = program.lock();
	findField("dcymd").lock()->setText(decayModes[sampler.lock()->getLastNp(lProgram.get())->getDecayMode()]);
}

void PgmParamsScreen::displayVoiceOverlap()
{
	init();
	auto lProgram = program.lock();
	findField("voiceoverlap").lock()->setText(voiceOverlapModes[sampler.lock()->getLastNp(lProgram.get())->getVoiceOverlap()]);
}
