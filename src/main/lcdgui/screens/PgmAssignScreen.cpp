#include "PgmAssignScreen.hpp"

#include "SelectDrumScreen.hpp"

#include "controller/ClientEventController.hpp"
#include "sampler/Pad.hpp"

using namespace mpc::lcdgui::screens;

PgmAssignScreen::PgmAssignScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "program-assign", layerIndex)
{
}

void PgmAssignScreen::open()
{
    auto program = getProgramOrThrow();
    auto selectedPad =  program->getPad(mpc.clientEventController->getSelectedPad());
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto soundIndex = selectedNoteParameters->getSoundIndex();

    if (soundIndex != -1)
    {
        sampler->setSoundIndex(soundIndex);
    }

    findField("pad-assign")->setAlignment(Alignment::Centered);
    findField("pad-assign")->setLocation(194, 11);

    mpc.clientEventController->addObserver(this);
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
    mpc.clientEventController->deleteObserver(this);
}

void PgmAssignScreen::function(int i)
{

    switch (i)
    {
        case 0:
        {
            auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "program-assign";
            mpc.getLayeredScreen()->openScreen<SelectDrumScreen>();
            break;
        }
        case 1:
            mpc.getLayeredScreen()->openScreen<PgmParamsScreen>();
            break;
        case 2:
            mpc.getLayeredScreen()->openScreen<DrumScreen>();
            break;
        case 3:
            mpc.getLayeredScreen()->openScreen<PurgeScreen>();
            break;
        case 4:
        {
            mpc.getLayeredScreen()->openScreen<AutoChromaticAssignmentScreen>();
            break;
        }
    }
}

void PgmAssignScreen::turnWheel(int i)
{
    auto program = getProgramOrThrow();

    auto selectedPad =  program->getPad(mpc.clientEventController->getSelectedPad());
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "pad-assign")
    {
        padAssign = i > 0;
        displayPadAssign();
        displayPadNote();
        displayNote();
        displayPad();
        displaySoundGenerationMode();
        displaySoundName();
    }
    else if (focusedFieldName == "pgm")
    {
        auto pgm = activeDrum().getProgram();
        auto candidate = sampler->getUsedProgram(pgm, i > 0);

        if (candidate != pgm)
        {
            activeDrum().setProgram(candidate);
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
    else if (focusedFieldName == "pad")
    {
        auto candidate = mpc.clientEventController->getSelectedPad() + i;

        if (candidate < 0 || candidate > 63)
        {
            return;
        }

        auto nextNote = program->getPad(candidate)->getNote();
        mpc.clientEventController->setSelectedNote(nextNote);
        mpc.clientEventController->setSelectedPad(candidate);
        displayPad();
        displayNote();
        displayOptionalNoteA();
        displayOptionalNoteB();
        displayPadNote();
        displayPgm();
        displaySoundGenerationMode();
        displaySoundName();
    }
    else if (focusedFieldName == "pad-note")
    {
        selectedPad->setNote(selectedPad->getNote() + i);

        mpc.clientEventController->setSelectedNote(selectedPad->getNote());

        displayPad();
        displayNote();
        displayOptionalNoteA();
        displayOptionalNoteB();
        displayPadNote();
        displayPgm();
        displaySoundGenerationMode();
        displaySoundName();
    }
    else if (focusedFieldName == "note")
    {
        auto candidate = mpc.clientEventController->getSelectedNote() + i;

        if (candidate < 35)
        {
            candidate = 35;
        }

        mpc.clientEventController->setSelectedNote(candidate);
        displayNote();
        displaySoundName();
    }
    else if (focusedFieldName == "snd")
    {
        auto currentSoundIndex = selectedNoteParameters->getSoundIndex();

        if (currentSoundIndex == -1 && (i < 0 || sampler->getSoundCount() == 0))
        {
            return;
        }

        if (currentSoundIndex == 0 && i < 0)
        {
            selectedNoteParameters->setSoundIndex(-1);
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

        selectedNoteParameters->setSoundIndex(nextMemoryIndex);

        sampler->setSoundIndex(nextMemoryIndex);

        displaySoundName();
    }
    else if (focusedFieldName == "mode")
    {
        selectedNoteParameters->setSoundGenMode(selectedNoteParameters->getSoundGenerationMode() + i);
        displaySoundGenerationMode();
    }
    else if (focusedFieldName == "velocity-range-lower")
    {
        selectedNoteParameters->setVeloRangeLower(selectedNoteParameters->getVelocityRangeLower() + i);
        displayVeloRangeLower();
        displayVeloRangeUpper();
    }
    else if (focusedFieldName == "velocity-range-upper")
    {
        selectedNoteParameters->setVeloRangeUpper(selectedNoteParameters->getVelocityRangeUpper() + i);
        displayVeloRangeLower();
        displayVeloRangeUpper();
    }
    else if (focusedFieldName == "optional-note-a")
    {
        selectedNoteParameters->setOptNoteA(selectedNoteParameters->getOptionalNoteA() + i);
        displayOptionalNoteA();
    }
    else if (focusedFieldName == "optional-note-b")
    {
        selectedNoteParameters->setOptionalNoteB(selectedNoteParameters->getOptionalNoteB() + i);
        displayOptionalNoteB();
    }
}

void PgmAssignScreen::openWindow()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "pgm")
    {
        mpc.getLayeredScreen()->openScreen<ProgramScreen>();
    }
    else if (focusedFieldName == "pad" || focusedFieldName == "pad-note")
    {
        mpc.getLayeredScreen()->openScreen<AssignmentViewScreen>();
    }
    else if (focusedFieldName == "pad-assign")
    {
        mpc.getLayeredScreen()->openScreen<InitPadAssignScreen>();
    }
    else if (focusedFieldName == "note")
    {
        mpc.getLayeredScreen()->openScreen<CopyNoteParametersScreen>();
    }
    else if (focusedFieldName == "snd")
    {
        auto program = getProgramOrThrow();
        auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
        auto soundIndex = selectedNoteParameters->getSoundIndex();

        if (soundIndex != -1)
        {
            sampler->setSoundIndex(soundIndex);
            sampler->setPreviousScreenName("program-assign");
            mpc.getLayeredScreen()->openScreen<SoundScreen>();
        }
    }
}

void PgmAssignScreen::displayPgm()
{
    auto program = getProgramOrThrow();
    findField("pgm")->setText(StrUtil::padLeft(std::to_string(activeDrum().getProgram() + 1), " ", 2) + "-" + program->getName());
}

void PgmAssignScreen::displaySoundName()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto soundIndex = selectedNoteParameters->getSoundIndex();

    if (soundIndex == -1)
    {
        findField("snd")->setText("OFF");
        findLabel("issoundstereo")->setText("    ");
    }
    else
    {
        auto name = sampler->getSoundName(soundIndex);
        findField("snd")->setText(name);
    }

    if (sampler->getSoundCount() != 0 && soundIndex != -1)
    {
        if (sampler->getSound(soundIndex)->isMono())
        {
            findLabel("issoundstereo")->setText("    ");
        }
        else
        {
            findLabel("issoundstereo")->setText("(ST)");
        }
    }
}

void PgmAssignScreen::displayPadAssign()
{
    findField("pad-assign")->setText(padAssign ? "MASTER" : "PROGRAM");
}

void PgmAssignScreen::displayPadNote()
{

    auto program = getProgramOrThrow();
    auto selectedPad =  program->getPad(mpc.clientEventController->getSelectedPad());

    if (selectedPad->getNote() == 34)
    {
        findField("pad-note")->setText("--");
    }
    else
    {
        findField("pad-note")->setText(std::to_string(selectedPad->getNote()));
    }
}

void PgmAssignScreen::displaySoundGenerationMode()
{
    auto sgm = -1;

    auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());

    if (selectedNoteParameters != nullptr)
    {
        sgm = selectedNoteParameters->getSoundGenerationMode();
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

    if (selectedNoteParameters == nullptr || sgm == -1 || sgm == 0)
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
    auto program = getProgramOrThrow();

    const auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto rangeB = selectedNoteParameters->getVelocityRangeUpper();
    findField("velocity-range-upper")->setTextPadded(rangeB, " ");
}

void PgmAssignScreen::displayVeloRangeLower()
{
    auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto rangeA = selectedNoteParameters->getVelocityRangeLower();
    findField("velocity-range-lower")->setTextPadded(rangeA, " ");
}

void PgmAssignScreen::displayOptionalNoteA()
{
    auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto noteIntA = selectedNoteParameters->getOptionalNoteA();
    auto padIntA = program->getPadIndexFromNote(noteIntA);
    auto noteA = noteIntA != 34 ? std::to_string(noteIntA) : "--";
    auto padA = sampler->getPadName(padIntA);
    findField("optional-note-a")->setText(noteA + "/" + padA);
}

void PgmAssignScreen::displayOptionalNoteB()
{
    auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto noteIntB = selectedNoteParameters->getOptionalNoteB();
    auto padIntB = program->getPadIndexFromNote(noteIntB);
    auto noteB = noteIntB != 34 ? std::to_string(noteIntB) : "--";
    auto padB = sampler->getPadName(padIntB);
    findField("optional-note-b")->setText(noteB + "/" + padB);
}

void PgmAssignScreen::displayNote()
{
    auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    findField("note")->setText(std::to_string(selectedNoteParameters->getNumber()));
}

void PgmAssignScreen::displayPad()
{
    findField("pad")->setText(sampler->getPadName(mpc.clientEventController->getSelectedPad()));
}

void PgmAssignScreen::update(Observable *o, Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        displayNote();
        displaySoundName();
    }
    else if (msg == "pad")
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
