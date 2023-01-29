#include "PgmAssignScreen.hpp"

#include "SelectDrumScreen.hpp"

#include <sampler/Pad.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace moduru::lang;

PgmAssignScreen::PgmAssignScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "program-assign", layerIndex)
{
}

void PgmAssignScreen::open()
{
    init();
    auto lastNoteParameters = sampler->getLastNp(program.get());
    auto soundIndex = lastNoteParameters->getSoundIndex();

    if (soundIndex != -1)
    {
        sampler->setSoundIndex(soundIndex);
    }

    findField("pad-assign")->setAlignment(Alignment::Centered);
	findField("pad-assign")->setLocation(194, 11);

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

	auto lastPad = sampler->getLastPad(program.get());
	auto lastNoteParameters = sampler->getLastNp(program.get());

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

		auto nextNote = program->getPad(candidate)->getNote();
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
        auto currentSoundIndex = lastNoteParameters->getSoundIndex();

        if (currentSoundIndex == -1 && (i < 0 || sampler->getSoundCount() == 0))
        {
            return;
        }

        if (currentSoundIndex == 0 && i < 0)
        {
            lastNoteParameters->setSoundIndex(-1);
            displaySoundName();
            return;
        }

        auto currentSound = sampler->getSound(currentSoundIndex);
        auto sortedSounds = sampler->getSortedSounds();

        int indexInSortedSounds = -1;

        for (size_t idx = 0; idx < sortedSounds.size(); idx++)
        {
            if (sortedSounds[idx].first == currentSound)
            {
                indexInSortedSounds = idx;
                break;
            }
        }

        auto nextSortedIndex = indexInSortedSounds;

        if (i < 0)
        {
            if (--nextSortedIndex < 0)
            {
                nextSortedIndex = 0;
            }
        }
        else
        {
            if (++nextSortedIndex >= sortedSounds.size())
            {
                nextSortedIndex = sortedSounds.size() - 1;
            }
        }

        auto nextMemoryIndex = sortedSounds[nextSortedIndex].second;

        lastNoteParameters->setSoundIndex(nextMemoryIndex);

        sampler->setSoundIndex(nextMemoryIndex);

		displaySoundName();
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
		auto soundIndex = sampler->getLastNp(program.get())->getSoundIndex();

		if (soundIndex != -1)
		{
            sampler->setSoundIndex(soundIndex);
			sampler->setPreviousScreenName("program-assign");
			openScreen("sound");
		}
	}
}

void PgmAssignScreen::displayPgm()
{
	findField("pgm")->setText(StrUtil::padLeft(std::to_string(mpcSoundPlayerChannel->getProgram() + 1), " ", 2) + "-" + program->getName());
}

void PgmAssignScreen::displaySoundName()
{
	int sndNumber = sampler->getLastNp(program.get())->getSoundIndex();

	if (sndNumber == -1)
	{
		findField("snd")->setText("OFF");
		findLabel("issoundstereo")->setText("");
	}
	else
	{
		auto name = sampler->getSoundName(sndNumber);
		findField("snd")->setText(name);
	}

	if (sampler->getSoundCount() != 0 && sndNumber != -1)
	{
		if (sampler->getSound(sndNumber)->isMono())
			findLabel("issoundstereo")->setText("");
		else
			findLabel("issoundstereo")->setText("(ST)");
	}
}

void PgmAssignScreen::displayPadAssign()
{
	init();
	findField("pad-assign")->setText(padAssign ? "MASTER" : "PROGRAM");
}

void PgmAssignScreen::displayPadNote()
{
	init();

	if (sampler->getLastPad(program.get())->getNote() == 34)
		findField("pad-note")->setText("--");
	else
		findField("pad-note")->setText(std::to_string(sampler->getLastPad(program.get())->getNote()));
}

void PgmAssignScreen::displaySoundGenerationMode()
{
	init();
	auto sgm = -1;

	if (sampler->getLastNp(program.get()) != nullptr)
	{
		sgm = sampler->getLastNp(program.get())->getSoundGenerationMode();
		findField("mode")->setText(soundGenerationModes[sgm]);
		
		if (sgm != 0)
		{
			findLabel("velocity-range-lower")->Hide(true);
			findField("velocity-range-lower")->Hide(true);
			findLabel("velocity-range-upper")->Hide(true);
			findField("velocity-range-upper")->Hide(true);
			findField("optional-note-a")->Hide(false);
			findLabel("optional-note-a")->Hide(false);
			findLabel("optional-note-b")->Hide(false);
			findField("optional-note-b")->Hide(false);
			findLabel("optional-note-a")->setText("Also play note:");
			findLabel("optional-note-b")->setText("Also play note:");
			displayOptionalNoteA();
			displayOptionalNoteB();
		}
		
		if (sgm == 2 || sgm == 3)
		{
			findLabel("optional-note-a")->setText("over:    , use:");
			findLabel("optional-note-b")->setText("over:    , use:");
			
			findLabel("velocity-range-lower")->Hide(false);
			findField("velocity-range-lower")->Hide(false);
			findLabel("velocity-range-upper")->Hide(false);
			findField("velocity-range-upper")->Hide(false);
			bringToFront(findField("velocity-range-lower").get());
			bringToFront(findField("velocity-range-upper").get());
			
			displayVeloRangeLower();
			displayVeloRangeUpper();
		}
	}

	if (sampler->getLastNp(program.get()) == nullptr || sgm == -1 || sgm == 0)
	{
		findLabel("velocity-range-lower")->Hide(true);
		findField("velocity-range-lower")->Hide(true);
		findLabel("velocity-range-upper")->Hide(true);
		findField("velocity-range-upper")->Hide(true);
		findLabel("optional-note-a")->Hide(true);
		findField("optional-note-a")->Hide(true);
		findLabel("optional-note-b")->Hide(true);
		findField("optional-note-b")->Hide(true);
	}
}

void PgmAssignScreen::displayVeloRangeUpper()
{
	init();
	auto rangeB = sampler->getLastNp(program.get())->getVelocityRangeUpper();
	findField("velocity-range-upper")->setTextPadded(rangeB, " ");
}

void PgmAssignScreen::displayVeloRangeLower()
{
	init();
	auto rangeA = sampler->getLastNp(program.get())->getVelocityRangeLower();
	findField("velocity-range-lower")->setTextPadded(rangeA, " ");
}

void PgmAssignScreen::displayOptionalNoteA()
{
	init();
	auto noteIntA = sampler->getLastNp(program.get())->getOptionalNoteA();
	auto padIntA = program->getPadIndexFromNote(noteIntA);
	auto noteA = noteIntA != 34 ? std::to_string(noteIntA) : "--";
	auto padA = sampler->getPadName(padIntA);
	findField("optional-note-a")->setText(noteA + "/" + padA);
}

void PgmAssignScreen::displayOptionalNoteB()
{
	init();
	auto noteIntB = sampler->getLastNp(program.get())->getOptionalNoteB();
	auto padIntB = program->getPadIndexFromNote(noteIntB);
	auto noteB = noteIntB != 34 ? std::to_string(noteIntB) : "--";
	auto padB = sampler->getPadName(padIntB);
	findField("optional-note-b")->setText(noteB + "/" + padB);
}

void PgmAssignScreen::displayNote()
{
	init();
	findField("note")->setText(std::to_string(sampler->getLastNp(program.get())->getNumber()));
}

void PgmAssignScreen::displayPad()
{
	init();
	findField("pad")->setText(sampler->getPadName(mpc.getPad()));
}

void PgmAssignScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto s = nonstd::any_cast<std::string>(arg);

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

void PgmAssignScreen::setPadAssign(bool isMaster)
{
    padAssign = isMaster;
}
