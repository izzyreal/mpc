#include "PgmAssignScreen.hpp"

#include "SelectDrumScreen.hpp"

#include <sampler/Pad.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

PgmAssignScreen::PgmAssignScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "program-assign", layerIndex)
{
}

void PgmAssignScreen::open()
{
	findField("pad-assign").lock()->setAlignment(Alignment::Centered);
	findField("pad-assign").lock()->setLocation(194, 11);

	mpc.addObserver(this);
	displayNote();
	displayOptionalNoteA();
	displayOptionalNoteB();
	displayPad();
	displayPadAssign();
	displayPadNote();
	displayPgm();
	displaySoundGenerationMode();
	displaySoundName();
	displayVeloRangeLower();
	displayVeloRangeUpper();
}

void PgmAssignScreen::close()
{
	mpc.deleteObserver(this);
}

void PgmAssignScreen::function(int i)
{
	init();
		
	switch (i)
	{
	case 0:	
	{
		auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>("select-drum");
		selectDrumScreen->redirectScreen = "program-assign";
		openScreen("select-drum");
		break;
	}
	case 1:
		openScreen("program-params");
		break;
	case 2:
		openScreen("drum");
		break;
	case 3:
		openScreen("purge");
		break;
	case 4:
	{
		mpc.setPreviousSamplerScreenName("program-assign");
		openScreen("auto-chromatic-assignment");
		break;
	}
	}
}

void PgmAssignScreen::turnWheel(int i)
{
	init();

	auto lastPad = sampler->getLastPad(program.lock().get());
	auto lastNoteParameters = sampler->getLastNp(program.lock().get());

	if (param == "pad-assign")
	{
		padAssign = i > 0;
		displayPadAssign();
		displayPadNote();
		displayNote();
		displayPad();
		displaySoundGenerationMode();
		displaySoundName();
	}
	else if (param == "pgm")
	{
		auto pgm = mpcSoundPlayerChannel->getProgram();
		auto candidate = sampler->getUsedProgram(pgm, i > 0);

		if (candidate != pgm)
		{
			mpcSoundPlayerChannel->setProgram(candidate);
			displayNote();
			displayOptionalNoteA();
			displayOptionalNoteB();
			displayPad();
			displayPadAssign();
			displayPadNote();
			displayPgm();
			displaySoundGenerationMode();
			displaySoundName();
			displayVeloRangeLower();
			displayVeloRangeUpper();
		}
	}
	else if (param == "pad")
	{
		auto candidate = mpc.getPad() + i;
		
		if (candidate < 0 || candidate > 63)
			return;

		auto nextNote = program.lock()->getPad(candidate)->getNote();
		mpc.setNote(nextNote);
		mpc.setPad(candidate);
		displayPad();
		displayNote();
		displayOptionalNoteA();
		displayOptionalNoteB();
		displayPadNote();
		displayPgm();
		displaySoundGenerationMode();
		displaySoundName();
	}
	else if (param == "pad-note")
	{
		lastPad->setNote(lastPad->getNote() + i);

        mpc.setNote(lastPad->getNote());

		displayPad();
		displayNote();
		displayOptionalNoteA();
		displayOptionalNoteB();
		displayPadNote();
		displayPgm();
		displaySoundGenerationMode();
		displaySoundName();
	}
	else if (param == "note")
	{
		auto candidate = mpc.getNote() + i;
	
		if (candidate < 35) {
            candidate = 35;
        }
        mpc.setNote(candidate);
        displayNote();
        displaySoundName();
    }
	else if (param == "snd")
 {		
		lastNoteParameters->setSoundIndex(sampler->getNextSoundIndex(lastNoteParameters->getSoundIndex(), i > 0));

		displaySoundName();

		auto sound = sampler->getSound(lastNoteParameters->getSoundIndex()).lock();
		
		if (sound)
		{
			if (sound->isMono())
				lastNoteParameters->getStereoMixerChannel().lock()->setStereo(false);
        }
	}
	else if (param == "mode")
	{
		lastNoteParameters->setSoundGenMode(lastNoteParameters->getSoundGenerationMode() + i);
		displaySoundGenerationMode();
	}
	else if (param == "velocity-range-lower")
	{
		lastNoteParameters->setVeloRangeLower(lastNoteParameters->getVelocityRangeLower() + i);
		displayVeloRangeLower();
	}
	else if (param == "velocity-range-upper")
	{
		lastNoteParameters->setVeloRangeUpper(lastNoteParameters->getVelocityRangeUpper() + i);
		displayVeloRangeUpper();
	}
	else if (param == "optional-note-a")
	{
		lastNoteParameters->setOptNoteA(lastNoteParameters->getOptionalNoteA() + i);
		displayOptionalNoteA();
	}
	else if (param == "optional-note-b")
	{
		lastNoteParameters->setOptionalNoteB(lastNoteParameters->getOptionalNoteB() + i);
		displayOptionalNoteB();
	}
}

void PgmAssignScreen::openWindow()
{
	init();
	
	if (param == "pgm")
	{
		mpc.setPreviousSamplerScreenName("program-assign");
		openScreen("program");
	}
	else if (param == "pad" || param == "pad-note")
	{
		openScreen("assignment-view");
	}
	else if (param == "pad-assign")
	{
		openScreen("init-pad-assign");
	}
	else if (param == "note")
	{
		openScreen("copy-note-parameters");
	}
	else if (param == "snd")
	{
		auto sn = sampler->getLastNp(program.lock().get())->getSoundIndex();
		if (sn != -1)
		{
			sampler->setSoundIndex(sn);
			sampler->setPreviousScreenName("program-assign");
			openScreen("sound");
		}
	}
}

void PgmAssignScreen::displayPgm()
{
	findField("pgm").lock()->setText(StrUtil::padLeft(to_string(mpcSoundPlayerChannel->getProgram() + 1), " ", 2) + "-" + program.lock()->getName());
}

void PgmAssignScreen::displaySoundName()
{
	int sndNumber = sampler->getLastNp(program.lock().get())->getSoundIndex();

	if (sndNumber == -1)
	{
		findField("snd").lock()->setText("OFF");
		findLabel("issoundstereo").lock()->setText("");
	}
	else
	{
		string name = sampler->getSoundName(sndNumber);
		findField("snd").lock()->setText(name);
	}

	if (sampler->getSoundCount() != 0 && sndNumber != -1)
	{
		if (sampler->getSound(sndNumber).lock()->isMono())
			findLabel("issoundstereo").lock()->setText("");
		else
			findLabel("issoundstereo").lock()->setText("(ST)");
	}
}

void PgmAssignScreen::displayPadAssign()
{
	init();
	findField("pad-assign").lock()->setText(padAssign ? "MASTER" : "PROGRAM");
}

void PgmAssignScreen::displayPadNote()
{
	init();
	auto lProgram = program.lock();

	if (sampler->getLastPad(lProgram.get())->getNote() == 34)
		findField("pad-note").lock()->setText("--");
	else
		findField("pad-note").lock()->setText(to_string(sampler->getLastPad(lProgram.get())->getNote()));
}

void PgmAssignScreen::displaySoundGenerationMode()
{
	init();
	auto sgm = -1;
	auto lProgram = program.lock();
	
	if (sampler->getLastNp(lProgram.get()) != nullptr)
	{
		sgm = sampler->getLastNp(lProgram.get())->getSoundGenerationMode();
		findField("mode").lock()->setText(soundGenerationModes[sgm]);
		
		if (sgm != 0)
		{
			findLabel("velocity-range-lower").lock()->Hide(true);
			findField("velocity-range-lower").lock()->Hide(true);
			findLabel("velocity-range-upper").lock()->Hide(true);
			findField("velocity-range-upper").lock()->Hide(true);
			findField("optional-note-a").lock()->Hide(false);
			findLabel("optional-note-a").lock()->Hide(false);
			findLabel("optional-note-b").lock()->Hide(false);
			findField("optional-note-b").lock()->Hide(false);
			findLabel("optional-note-a").lock()->setText("Also play note:");
			findLabel("optional-note-b").lock()->setText("Also play note:");
			displayOptionalNoteA();
			displayOptionalNoteB();
		}
		
		if (sgm == 2 || sgm == 3)
		{
			findLabel("optional-note-a").lock()->setText("over:    , use:");
			findLabel("optional-note-b").lock()->setText("over:    , use:");
			
			findLabel("velocity-range-lower").lock()->Hide(false);
			findField("velocity-range-lower").lock()->Hide(false);
			findLabel("velocity-range-upper").lock()->Hide(false);
			findField("velocity-range-upper").lock()->Hide(false);
			bringToFront(findField("velocity-range-lower").lock().get());
			bringToFront(findField("velocity-range-upper").lock().get());
			
			displayVeloRangeLower();
			displayVeloRangeUpper();
		}
	}

	if (sampler->getLastNp(lProgram.get()) == nullptr || sgm == -1 || sgm == 0)
	{
		findLabel("velocity-range-lower").lock()->Hide(true);
		findField("velocity-range-lower").lock()->Hide(true);
		findLabel("velocity-range-upper").lock()->Hide(true);
		findField("velocity-range-upper").lock()->Hide(true);
		findLabel("optional-note-a").lock()->Hide(true);
		findField("optional-note-a").lock()->Hide(true);
		findLabel("optional-note-b").lock()->Hide(true);
		findField("optional-note-b").lock()->Hide(true);
	}
}

void PgmAssignScreen::displayVeloRangeUpper()
{
	init();
	auto rangeB = sampler->getLastNp(program.lock().get())->getVelocityRangeUpper();
	findField("velocity-range-upper").lock()->setTextPadded(rangeB, " ");
}

void PgmAssignScreen::displayVeloRangeLower()
{
	init();
	auto rangeA = sampler->getLastNp(program.lock().get())->getVelocityRangeLower();
	findField("velocity-range-lower").lock()->setTextPadded(rangeA, " ");
}

void PgmAssignScreen::displayOptionalNoteA()
{
	init();
	auto lProgram = program.lock();
	auto noteIntA = sampler->getLastNp(lProgram.get())->getOptionalNoteA();
	auto padIntA = lProgram->getPadIndexFromNote(noteIntA);
	auto noteA = noteIntA != 34 ? to_string(noteIntA) : "--";
	auto padA = sampler->getPadName(padIntA);
	findField("optional-note-a").lock()->setText(noteA + "/" + padA);
}

void PgmAssignScreen::displayOptionalNoteB()
{
	init();
	auto lProgram = program.lock();
	auto noteIntB = sampler->getLastNp(lProgram.get())->getOptionalNoteB();
	auto padIntB = lProgram->getPadIndexFromNote(noteIntB);
	auto noteB = noteIntB != 34 ? to_string(noteIntB) : "--";
	auto padB = sampler->getPadName(padIntB);
	findField("optional-note-b").lock()->setText(noteB + "/" + padB);
}

void PgmAssignScreen::displayNote()
{
	init();
	findField("note").lock()->setText(to_string(sampler->getLastNp(program.lock().get())->getNumber()));
}

void PgmAssignScreen::displayPad()
{
	init();
	findField("pad").lock()->setText(sampler->getPadName(mpc.getPad()));
}

void PgmAssignScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s == "note")
	{
		displayNote();
		displaySoundName();
	}
	else if (s == "pad")
	{
		displayNote();
		displayPad();
		displayPadNote();
		displaySoundName();
		displaySoundGenerationMode();
	}
}
