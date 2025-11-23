#include "PgmAssignScreen.hpp"

#include "Mpc.hpp"
#include "SelectDrumScreen.hpp"
#include "StrUtil.hpp"

#include "controller/ClientEventController.hpp"
#include "lcdgui/Label.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens;

PgmAssignScreen::PgmAssignScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "program-assign", layerIndex)
{
}

void PgmAssignScreen::open()
{
    auto displayPadAndNoteParameters = [this]
    {
        displayNote();
        displayOptionalNoteA();
        displayOptionalNoteB();
        displayPad();
        displayPadNote();
        displaySoundGenerationMode();
        displaySoundName();
        displayVeloRangeLower();
        displayVeloRangeUpper();
    };

    if (isReactiveBindingsEmpty())
    {
        auto getSelectedNote = [this]
        {
            return this->mpc.clientEventController->getSelectedNote();
        };
        auto getSelectedPadIndex = [this]
        {
            return this->mpc.clientEventController->getSelectedPad();
        };
        auto getSelectedPad = [this, getSelectedPadIndex]
        {
            return getProgramOrThrow()->getPad(getSelectedPadIndex());
        };

        auto getSelectedNoteParameters = [this, getSelectedNote]
        {
            return getProgramOrThrow()->getNoteParameters(getSelectedNote());
        };

        addReactiveBinding({[getSelectedNote]
                            {
                                return getSelectedNote();
                            },
                            [displayPadAndNoteParameters](auto)
                            {
                                displayPadAndNoteParameters();
                            }});

        addReactiveBinding(
            {[getSelectedNoteParameters, this]
             {
                 if (!getProgram())
                 {
                     return -1;
                 }
                 return getSelectedNoteParameters()->getSoundIndex();
             },
             [this](auto)
             {
                 displaySoundName();
             }});

        addReactiveBinding({[getSelectedPad]
                            {
                                return getSelectedPad()->getNote();
                            },
                            [displayPadAndNoteParameters](auto)
                            {
                                displayPadAndNoteParameters();
                            }});

        addReactiveBinding({[this]
                            {
                                return getActiveDrumBus()->getProgramIndex();
                            },
                            [displayPadAndNoteParameters, this](auto)
                            {
                                displayPgm();
                                displayPadAndNoteParameters();
                            }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getSoundGenerationMode();
             },
             [this](auto)
             {
                 displaySoundGenerationMode();
             }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getVelocityRangeLower();
             },
             [this](auto)
             {
                 displayVeloRangeLower();
             }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getVelocityRangeUpper();
             },
             [this](auto)
             {
                 displayVeloRangeUpper();
             }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getOptionalNoteA();
             },
             [this](auto)
             {
                 displayOptionalNoteA();
             }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getOptionalNoteB();
             },
             [this](auto)
             {
                 displayOptionalNoteB();
             }});
    }

    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto soundIndex = selectedNoteParameters->getSoundIndex();
        soundIndex != -1)
    {
        sampler.lock()->setSoundIndex(soundIndex);
    }

    findField("pad-assign")->setAlignment(Alignment::Centered);
    findField("pad-assign")->setLocation(194, 11);

    displayPadAssign();
    displayPgm();

    displayPadAndNoteParameters();
}

void PgmAssignScreen::function(const int i)
{
    switch (i)
    {
        case 0:
        {
            const auto selectDrumScreen =
                mpc.screens->get<ScreenId::SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "program-assign";
            openScreenById(ScreenId::SelectDrumScreen);
            break;
        }
        case 1:
            openScreenById(ScreenId::PgmParamsScreen);
            break;
        case 2:
            openScreenById(ScreenId::DrumScreen);
            break;
        case 3:
            openScreenById(ScreenId::PurgeScreen);
            break;
        case 4:
        {
            openScreenById(ScreenId::AutoChromaticAssignmentScreen);
            break;
        }
        default:;
    }
}

void PgmAssignScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();

    const auto selectedPad =
        program->getPad(mpc.clientEventController->getSelectedPad());
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "pad-assign")
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
        const auto pgm = getActiveDrumBus()->getProgramIndex();

        if (const auto candidate = sampler.lock()->getUsedProgram(pgm, i > 0);
            candidate != pgm)
        {
            getActiveDrumBus()->setProgramIndex(candidate);
        }
    }
    else if (focusedFieldName == "pad")
    {
        const auto candidate = mpc.clientEventController->getSelectedPad() + i;

        if (candidate < 0 || candidate > 63)
        {
            return;
        }

        const auto nextNote = program->getPad(candidate)->getNote();
        mpc.clientEventController->setSelectedNote(nextNote);
        mpc.clientEventController->setSelectedPad(candidate);
    }
    else if (focusedFieldName == "pad-note")
    {
        selectedPad->setNote(selectedPad->getNote() + i);

        mpc.clientEventController->setSelectedNote(selectedPad->getNote());
    }
    else if (focusedFieldName == "note")
    {
        auto candidate = mpc.clientEventController->getSelectedNote() + i;

        if (candidate < MinDrumNoteNumber)
        {
            candidate = MinDrumNoteNumber;
        }

        mpc.clientEventController->setSelectedNote(candidate);
    }
    else if (focusedFieldName == "snd")
    {
        const auto currentSoundIndex = selectedNoteParameters->getSoundIndex();

        if (currentSoundIndex == -1 && (i < 0 || sampler.lock()->getSoundCount() == 0))
        {
            return;
        }

        if (currentSoundIndex == 0 && i < 0)
        {
            selectedNoteParameters->setSoundIndex(-1);
            return;
        }

        const auto currentSound = sampler.lock()->getSound(currentSoundIndex);
        const auto sortedSounds = sampler.lock()->getSortedSounds();

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

        const auto nextMemoryIndex = sortedSounds[nextSortedIndex].second;
        selectedNoteParameters->setSoundIndex(nextMemoryIndex);
        sampler.lock()->setSoundIndex(nextMemoryIndex);
    }
    else if (focusedFieldName == "mode")
    {
        selectedNoteParameters->setSoundGenMode(
            selectedNoteParameters->getSoundGenerationMode() + i);
    }
    else if (focusedFieldName == "velocity-range-lower")
    {
        selectedNoteParameters->setVeloRangeLower(
            selectedNoteParameters->getVelocityRangeLower() + i);
    }
    else if (focusedFieldName == "velocity-range-upper")
    {
        selectedNoteParameters->setVeloRangeUpper(
            selectedNoteParameters->getVelocityRangeUpper() + i);
    }
    else if (focusedFieldName == "optional-note-a")
    {
        selectedNoteParameters->setOptionalNoteA(
            selectedNoteParameters->getOptionalNoteA() + i);
    }
    else if (focusedFieldName == "optional-note-b")
    {
        selectedNoteParameters->setOptionalNoteB(
            selectedNoteParameters->getOptionalNoteB() + i);
    }
}

void PgmAssignScreen::openWindow()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "pgm")
    {
        openScreenById(ScreenId::ProgramScreen);
    }
    else if (focusedFieldName == "pad" || focusedFieldName == "pad-note")
    {
        openScreenById(ScreenId::AssignmentViewScreen);
    }
    else if (focusedFieldName == "pad-assign")
    {
        openScreenById(ScreenId::InitPadAssignScreen);
    }
    else if (focusedFieldName == "note")
    {
        openScreenById(ScreenId::CopyNoteParametersScreen);
    }
    else if (focusedFieldName == "snd")
    {
        const auto program = getProgramOrThrow();
        const auto selectedNoteParameters = program->getNoteParameters(
            mpc.clientEventController->getSelectedNote());

        if (const auto soundIndex = selectedNoteParameters->getSoundIndex();
            soundIndex != -1)
        {
            sampler.lock()->setSoundIndex(soundIndex);
            sampler.lock()->setPreviousScreenName("program-assign");
            openScreenById(ScreenId::SoundScreen);
        }
    }
}

void PgmAssignScreen::displayPgm() const
{
    const auto program = getProgramOrThrow();
    findField("pgm")->setText(
        StrUtil::padLeft(
            std::to_string(getActiveDrumBus()->getProgramIndex() + 1), " ", 2) +
        "-" + program->getName());
}

void PgmAssignScreen::displaySoundName() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto soundIndex = selectedNoteParameters->getSoundIndex();

    if (soundIndex == -1)
    {
        findField("snd")->setText("OFF");
        findLabel("issoundstereo")->setText("    ");
    }
    else
    {
        const auto name = sampler.lock()->getSoundName(soundIndex);
        findField("snd")->setText(name);
    }

    if (sampler.lock()->getSoundCount() != 0 && soundIndex != -1)
    {
        if (sampler.lock()->getSound(soundIndex)->isMono())
        {
            findLabel("issoundstereo")->setText("    ");
        }
        else
        {
            findLabel("issoundstereo")->setText("(ST)");
        }
    }
}

void PgmAssignScreen::displayPadAssign() const
{
    findField("pad-assign")->setText(padAssign ? "MASTER" : "PROGRAM");
}

void PgmAssignScreen::displayPadNote() const
{
    const auto program = getProgramOrThrow();
    const auto selectedPad =
        program->getPad(mpc.clientEventController->getSelectedPad());

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
    int soundGenerationMode = -1;

    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (selectedNoteParameters != nullptr)
    {
        soundGenerationMode = selectedNoteParameters->getSoundGenerationMode();
        findField("mode")->setText(soundGenerationModes[soundGenerationMode]);

        if (soundGenerationMode != 0)
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

        if (soundGenerationMode == 2 || soundGenerationMode == 3)
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

    if (selectedNoteParameters == nullptr || soundGenerationMode == -1 ||
        soundGenerationMode == 0)
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

void PgmAssignScreen::displayVeloRangeUpper() const
{
    const auto program = getProgramOrThrow();

    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto rangeB = selectedNoteParameters->getVelocityRangeUpper();
    findField("velocity-range-upper")->setTextPadded(rangeB, " ");
}

void PgmAssignScreen::displayVeloRangeLower() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto rangeA = selectedNoteParameters->getVelocityRangeLower();
    findField("velocity-range-lower")->setTextPadded(rangeA, " ");
}

void PgmAssignScreen::displayOptionalNoteA() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto noteIntA = selectedNoteParameters->getOptionalNoteA();
    const auto padIntA = program->getPadIndexFromNote(noteIntA);
    const auto noteA = noteIntA != 34 ? std::to_string(noteIntA) : "--";
    const auto padA = sampler.lock()->getPadName(padIntA);
    findField("optional-note-a")->setText(noteA + "/" + padA);
}

void PgmAssignScreen::displayOptionalNoteB() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto noteIntB = selectedNoteParameters->getOptionalNoteB();
    const auto padIntB = program->getPadIndexFromNote(noteIntB);
    const auto noteB = noteIntB != 34 ? std::to_string(noteIntB) : "--";
    const auto padB = sampler.lock()->getPadName(padIntB);
    findField("optional-note-b")->setText(noteB + "/" + padB);
}

void PgmAssignScreen::displayNote() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("note")->setText(
        std::to_string(selectedNoteParameters->getNumber()));
}

void PgmAssignScreen::displayPad() const
{
    findField("pad")->setText(
        sampler.lock()->getPadName(mpc.clientEventController->getSelectedPad()));
}

void PgmAssignScreen::setPadAssign(const bool isMaster)
{
    padAssign = isMaster;
}

bool PgmAssignScreen::isPadAssignMaster() const
{
    return padAssign;
}
