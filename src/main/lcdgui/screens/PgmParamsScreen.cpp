#include "PgmParamsScreen.hpp"

#include "Mpc.hpp"
#include "SelectDrumScreen.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/VoiceOverlapMode.hpp"

#include "sampler/NoteParameters.hpp"

#include "lcdgui/EnvGraph.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

PgmParamsScreen::PgmParamsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "program-params", layerIndex)
{
    addChildT<EnvGraph>(mpc);
}

void PgmParamsScreen::open()
{
    mpc.clientEventController->addObserver(this);
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
    mpc.clientEventController->deleteObserver(this);
}

void PgmParamsScreen::function(const int i)
{
    switch (i)
    {
        case 0:
            openScreenById(ScreenId::PgmAssignScreen);
            break;
        case 1:
        {
            const auto selectDrumScreen =
                mpc.screens->get<ScreenId::SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "program-params";
            openScreenById(ScreenId::SelectDrumScreen);
            break;
        }
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
        case 5:
            break;
        default:;
    }
}

void PgmParamsScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "tune")
    {
        selectedNoteParameters->setTune(selectedNoteParameters->getTune() + i);
        displayTune();
    }
    else if (focusedFieldName == "dcymd")
    {
        selectedNoteParameters->setDecayMode(
            selectedNoteParameters->getDecayMode() + i);
        displayDecayMode();
    }
    else if (focusedFieldName == "voiceoverlap")
    {
        const auto s =
            sampler->getSound(selectedNoteParameters->getSoundIndex());

        if (s && s->isLoopEnabled())
        {
            return;
        }

        using M = sampler::VoiceOverlapMode;

        int modeVal =
            static_cast<int>(selectedNoteParameters->getVoiceOverlapMode());
        modeVal += i;

        modeVal = std::clamp(modeVal, static_cast<int>(M::POLY),
                             static_cast<int>(M::NOTE_OFF));

        selectedNoteParameters->setVoiceOverlapMode(static_cast<M>(modeVal));

        displayVoiceOverlap();
    }
    else if (focusedFieldName == "reson")
    {
        selectedNoteParameters->setFilterResonance(
            selectedNoteParameters->getFilterResonance() + i);
        displayReson();
    }
    else if (focusedFieldName == "freq")
    {
        selectedNoteParameters->setFilterFrequency(
            selectedNoteParameters->getFilterFrequency() + i);
        displayFreq();
    }
    else if (focusedFieldName == "decay")
    {
        selectedNoteParameters->setDecay(selectedNoteParameters->getDecay() +
                                         i);
        displayAttackDecay();
    }
    else if (focusedFieldName == "attack")
    {
        selectedNoteParameters->setAttack(selectedNoteParameters->getAttack() +
                                          i);
        displayAttackDecay();
    }
    else if (focusedFieldName == "pgm")
    {
        const auto pgm = getActiveDrumBus()->getProgram();

        if (const auto candidate = sampler->getUsedProgram(pgm, i > 0);
            candidate != pgm)
        {
            getActiveDrumBus()->setProgram(candidate);
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
    else if (focusedFieldName == "note")
    {
        if (const auto candidate =
                mpc.clientEventController->getSelectedNote() + i;
            candidate > 34)
        {
            mpc.clientEventController->setSelectedNote(candidate);
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
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "pgm")
    {
        openScreenById(ScreenId::ProgramScreen);
    }
    else if (focusedFieldName == "note")
    {
        openScreenById(ScreenId::CopyNoteParametersScreen);
    }
    else if (focusedFieldName == "attack" || focusedFieldName == "decay" ||
             focusedFieldName == "dcymd")
    {
        openScreenById(ScreenId::VelocityModulationScreen);
    }
    else if (focusedFieldName == "freq" || focusedFieldName == "reson")
    {
        openScreenById(ScreenId::VeloEnvFilterScreen);
    }
    else if (focusedFieldName == "tune")
    {
        openScreenById(ScreenId::VeloPitchScreen);
    }
    else if (focusedFieldName == "voiceoverlap")
    {
        openScreenById(ScreenId::MuteAssignScreen);
    }
}

void PgmParamsScreen::update(Observable *observable, const Message message)
{
    if (const auto msg = std::get<std::string>(message); msg == "note")
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

void PgmParamsScreen::displayReson() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("reson")->setTextPadded(
        selectedNoteParameters->getFilterResonance());
}

void PgmParamsScreen::displayFreq() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("freq")->setTextPadded(
        selectedNoteParameters->getFilterFrequency());
}

void PgmParamsScreen::displayAttackDecay()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto attack = selectedNoteParameters->getAttack();
    const auto decay = selectedNoteParameters->getDecay();
    const auto decayModeStart = selectedNoteParameters->getDecayMode() == 1;
    findField("attack")->setTextPadded(attack);
    findField("decay")->setTextPadded(decay);
    findEnvGraph()->setCoordinates(attack, decay, decayModeStart);
}

void PgmParamsScreen::displayNote() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto soundIndex = selectedNoteParameters->getSoundIndex();
    const auto padIndex =
        program->getPadIndexFromNote(selectedNoteParameters->getNumber());
    const auto padName = sampler->getPadName(padIndex);
    const auto sampleName =
        soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    const std::string stereo =
        soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)"
                                                                     : "";
    findField("note")->setText(
        std::to_string(selectedNoteParameters->getNumber()) + "/" + padName +
        "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void PgmParamsScreen::displayPgm() const
{
    findField("pgm")->setTextPadded(getActiveDrumBus()->getProgram() + 1, " ");
}

void PgmParamsScreen::displayTune() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto tune = selectedNoteParameters->getTune();
    const auto sign = tune < 0 ? "-" : " ";
    const auto number = StrUtil::padLeft(std::to_string(abs(tune)), " ", 3);
    findField("tune")->setText(sign + number);
}

void PgmParamsScreen::displayDecayMode()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("dcymd")->setText(
        decayModes[selectedNoteParameters->getDecayMode()]);
    displayAttackDecay();
}

void PgmParamsScreen::displayVoiceOverlap() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    sampler::VoiceOverlapMode mode =
        selectedNoteParameters->getVoiceOverlapMode();

    const auto sound =
        sampler->getSound(selectedNoteParameters->getSoundIndex());

    if (sound && sound->isLoopEnabled())
    {
        mode = sampler::VoiceOverlapMode::NOTE_OFF;
    }

    findField("voiceoverlap")
        ->setText(voiceOverlapModes[static_cast<int>(mode)]);
}
