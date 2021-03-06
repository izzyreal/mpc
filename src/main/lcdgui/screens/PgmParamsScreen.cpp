#include "PgmParamsScreen.hpp"

#include "SelectDrumScreen.hpp"

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/EnvGraph.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

PgmParamsScreen::PgmParamsScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "program-params", layerIndex)
{
	addChild(make_shared<EnvGraph>(mpc));
}

void PgmParamsScreen::open()
{
	mpc.addObserver(this);
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
		openScreen("program-assign");
		break;
	case 1:
	{	
		auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>("select-drum");
		selectDrumScreen->redirectScreen = "program-params";
		openScreen("select-drum");
		break;
	}
	case 2:
		openScreen("drum");
		break;
	case 3:
		openScreen("purge");
		break;
	case 4:
	{
		mpc.setPreviousSamplerScreenName("program-params");
		openScreen("auto-chromatic-assignment");
		break;
	}
	case 5:
		break;
	}
}

void PgmParamsScreen::turnWheel(int i)
{
    init();

	auto lastNoteParameters = sampler.lock()->getLastNp(program.lock().get());

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
        auto s = sampler.lock()->getSound(lastNoteParameters->getSoundIndex()).lock();
		
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
		mpc.setPreviousSamplerScreenName("program-params");
		openScreen("program");
	}
	else if (param.compare("note") == 0)
	{
		mpc.setPreviousSamplerScreenName("program-params");
		openScreen("copy-note-parameters");
	}
	else if (param.compare("attack") == 0 || param.compare("decay") == 0 || param.compare("dcymd") == 0)
	{
		openScreen("velocity-modulation");
	}
	else if (param.compare("freq") == 0 || param.compare("reson") == 0)
	{
		openScreen("velo-env-filter");
	}
	else if (param.compare("tune") == 0)
	{
		openScreen("velo-pitch");
	}
	else if (param.compare("voiceoverlap") == 0)
	{
		openScreen("mute-assign");
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
	findField("reson").lock()->setTextPadded(sampler.lock()->getLastNp(lProgram.get())->getFilterResonance());
}

void PgmParamsScreen::displayFreq()
{
	init();
	auto lProgram = program.lock();
	findField("freq").lock()->setTextPadded(sampler.lock()->getLastNp(lProgram.get())->getFilterFrequency());
}

void PgmParamsScreen::displayAttackDecay()
{
	init();
	auto lProgram = program.lock();
	auto attack = sampler.lock()->getLastNp(lProgram.get())->getAttack();
	auto decay = sampler.lock()->getLastNp(lProgram.get())->getDecay();
	auto decayModeStart = sampler.lock()->getLastNp(lProgram.get())->getDecayMode() == 1;
	findField("attack").lock()->setTextPadded(attack);
	findField("decay").lock()->setTextPadded(decay);
	findEnvGraph().lock()->setCoordinates(attack, decay, decayModeStart);
}

void PgmParamsScreen::displayNote()
{
	init();
	auto lProgram = program.lock();
	auto sampleNumber = sampler.lock()->getLastNp(lProgram.get())->getSoundIndex();
	auto noteParameters = sampler.lock()->getLastNp(lProgram.get());
	auto note = noteParameters->getNumber();
	auto sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "OFF";
	auto padNumber = lProgram->getPadIndexFromNote(note);
	
	if (padNumber != -1)
	{
		auto stereo = noteParameters->getStereoMixerChannel().lock()->isStereo() && sampleNumber != -1 ? "(ST)" : "";
		auto padName = sampler.lock()->getPadName(padNumber);
		findField("note").lock()->setText(to_string(note) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
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
	auto tune = sampler.lock()->getLastNp(lProgram.get())->getTune();
	auto sign = tune < 0 ? "-" : " ";
	auto number = StrUtil::padLeft(to_string(abs(tune)), " ", 3);
	findField("tune").lock()->setText(sign + number);
}

void PgmParamsScreen::displayDecayMode()
{
	init();
	auto lProgram = program.lock();
	findField("dcymd").lock()->setText(decayModes[sampler.lock()->getLastNp(lProgram.get())->getDecayMode()]);
	displayAttackDecay();
}

void PgmParamsScreen::displayVoiceOverlap()
{
	init();
	
    auto lProgram = program.lock();
    auto lastNoteParameters = sampler.lock()->getLastNp(lProgram.get());
    auto mode = lastNoteParameters->getVoiceOverlap();
    
    auto sound = sampler.lock()->getSound(lastNoteParameters->getSoundIndex()).lock();
    
    if (sound && sound->isLoopEnabled())
        mode = 2;
    
	findField("voiceoverlap").lock()->setText(voiceOverlapModes[mode]);
}
