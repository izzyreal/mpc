#include "PgmParamsScreen.hpp"

#include "SelectDrumScreen.hpp"

#include <sampler/NoteParameters.hpp>

#include <lcdgui/EnvGraph.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;

PgmParamsScreen::PgmParamsScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "program-params", layerIndex)
{
	addChildT<EnvGraph>(mpc);
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

	auto lastNoteParameters = sampler->getLastNp(program.get());

	if (param == "tune")
	{
		lastNoteParameters->setTune(lastNoteParameters->getTune() + i);
		displayTune();
	}
	else if (param == "dcymd")
	{
		lastNoteParameters->setDecayMode(lastNoteParameters->getDecayMode() + i);
		displayDecayMode();
	}
	else if(param == "voiceoverlap")
	{
        auto s = sampler->getSound(lastNoteParameters->getSoundIndex());
		
		if (s && s->isLoopEnabled())
		{
			return;
		}
        
		lastNoteParameters->setVoiceOverlap(lastNoteParameters->getVoiceOverlap() + i);
		displayVoiceOverlap();
    }
	else if (param == "reson")
	{
		lastNoteParameters->setFilterResonance(lastNoteParameters->getFilterResonance() + i);
		displayReson();
	}
	else if (param == "freq")
	{
		lastNoteParameters->setFilterFrequency(lastNoteParameters->getFilterFrequency() + i);
		displayFreq();
	}
	else if (param == "decay")
	{
		lastNoteParameters->setDecay(lastNoteParameters->getDecay() + i);
		displayAttackDecay();
	}
	else if (param == "attack")
	{
		lastNoteParameters->setAttack(lastNoteParameters->getAttack() + i);
		displayAttackDecay();
	}
	else if (param == "pgm")
	{	
		auto pgm = activeDrum().getProgram();
		auto candidate = sampler->getUsedProgram(pgm, i > 0);
		
		if (candidate != pgm)
		{
            activeDrum().setProgram(candidate);
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
    else if(param == "note")
	{
        auto candidate = mpc.getNote() + i;
		if (candidate > 34)
		{
			mpc.setNote(candidate);
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

	if (param == "pgm")
	{
		mpc.setPreviousSamplerScreenName("program-params");
		openScreen("program");
	}
	else if (param == "note")
	{
		mpc.setPreviousSamplerScreenName("program-params");
		openScreen("copy-note-parameters");
	}
	else if (param == "attack" || param == "decay" || param == "dcymd")
	{
		openScreen("velocity-modulation");
	}
	else if (param == "freq" || param == "reson")
	{
		openScreen("velo-env-filter");
	}
	else if (param == "tune")
	{
		openScreen("velo-pitch");
	}
	else if (param == "voiceoverlap")
	{
		openScreen("mute-assign");
	}
}

void PgmParamsScreen::update(Observable* o, Message message)
{
    const auto msg = std::get<std::string>(message);

	if (msg == "note")
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
	findField("reson")->setTextPadded(sampler->getLastNp(program.get())->getFilterResonance());
}

void PgmParamsScreen::displayFreq()
{
	init();
	findField("freq")->setTextPadded(sampler->getLastNp(program.get())->getFilterFrequency());
}

void PgmParamsScreen::displayAttackDecay()
{
	init();
	auto attack = sampler->getLastNp(program.get())->getAttack();
	auto decay = sampler->getLastNp(program.get())->getDecay();
	auto decayModeStart = sampler->getLastNp(program.get())->getDecayMode() == 1;
	findField("attack")->setTextPadded(attack);
	findField("decay")->setTextPadded(decay);
	findEnvGraph()->setCoordinates(attack, decay, decayModeStart);
}

void PgmParamsScreen::displayNote()
{
	init();
    auto noteParameters = sampler->getLastNp(program.get());
    auto soundIndex = noteParameters->getSoundIndex();
    auto padIndex = program->getPadIndexFromNote(noteParameters->getNumber());
    auto padName = sampler->getPadName(padIndex);
    auto sampleName = soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)" : "";
    findField("note")->setText(std::to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void PgmParamsScreen::displayPgm()
{
	init();
	findField("pgm")->setTextPadded(activeDrum().getProgram() + 1, " ");
}

void PgmParamsScreen::displayTune()
{
	init();
	auto tune = sampler->getLastNp(program.get())->getTune();
	auto sign = tune < 0 ? "-" : " ";
	auto number = StrUtil::padLeft(std::to_string(abs(tune)), " ", 3);
	findField("tune")->setText(sign + number);
}

void PgmParamsScreen::displayDecayMode()
{
	init();
	findField("dcymd")->setText(decayModes[sampler->getLastNp(program.get())->getDecayMode()]);
	displayAttackDecay();
}

void PgmParamsScreen::displayVoiceOverlap()
{
	init();
	
    auto lastNoteParameters = sampler->getLastNp(program.get());
    auto mode = lastNoteParameters->getVoiceOverlap();
    
    auto sound = sampler->getSound(lastNoteParameters->getSoundIndex());
    
    if (sound && sound->isLoopEnabled())
        mode = 2;
    
	findField("voiceoverlap")->setText(voiceOverlapModes[mode]);
}
