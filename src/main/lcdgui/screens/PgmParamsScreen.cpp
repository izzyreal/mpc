#include "PgmParamsScreen.hpp"

#include "SelectDrumScreen.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/VoiceOverlapMode.hpp"

#include "sampler/NoteParameters.hpp"

#include "lcdgui/EnvGraph.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

PgmParamsScreen::PgmParamsScreen(mpc::Mpc &mpc, const int layerIndex)
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

void PgmParamsScreen::function(int i)
{

    switch (i)
    {
        case 0:
            mpc.getLayeredScreen()->openScreen<PgmAssignScreen>();
            break;
        case 1:
        {
            auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "program-params";
            mpc.getLayeredScreen()->openScreen<SelectDrumScreen>();
            break;
        }
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
        case 5:
            break;
    }
}

void PgmParamsScreen::turnWheel(int i)
{

    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tune")
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
        auto s = sampler->getSound(selectedNoteParameters->getSoundIndex());

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
    else if (focusedFieldName == "note")
    {
        auto candidate = mpc.clientEventController->getSelectedNote() + i;
        if (candidate > 34)
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
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "pgm")
    {
        mpc.getLayeredScreen()->openScreen<ProgramScreen>();
    }
    else if (focusedFieldName == "note")
    {
        mpc.getLayeredScreen()->openScreen<CopyNoteParametersScreen>();
    }
    else if (focusedFieldName == "attack" || focusedFieldName == "decay" ||
             focusedFieldName == "dcymd")
    {
        mpc.getLayeredScreen()->openScreen<VelocityModulationScreen>();
    }
    else if (focusedFieldName == "freq" || focusedFieldName == "reson")
    {
        mpc.getLayeredScreen()->openScreen<VeloEnvFilterScreen>();
    }
    else if (focusedFieldName == "tune")
    {
        mpc.getLayeredScreen()->openScreen<VeloPitchScreen>();
    }
    else if (focusedFieldName == "voiceoverlap")
    {
        mpc.getLayeredScreen()->openScreen<MuteAssignScreen>();
    }
}

void PgmParamsScreen::update(Observable *o, Message message)
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
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("reson")->setTextPadded(
        selectedNoteParameters->getFilterResonance());
}

void PgmParamsScreen::displayFreq()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("freq")->setTextPadded(
        selectedNoteParameters->getFilterFrequency());
}

void PgmParamsScreen::displayAttackDecay()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto attack = selectedNoteParameters->getAttack();
    auto decay = selectedNoteParameters->getDecay();
    auto decayModeStart = selectedNoteParameters->getDecayMode() == 1;
    findField("attack")->setTextPadded(attack);
    findField("decay")->setTextPadded(decay);
    findEnvGraph()->setCoordinates(attack, decay, decayModeStart);
}

void PgmParamsScreen::displayNote()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto soundIndex = selectedNoteParameters->getSoundIndex();
    auto padIndex =
        program->getPadIndexFromNote(selectedNoteParameters->getNumber());
    auto padName = sampler->getPadName(padIndex);
    auto sampleName =
        soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo =
        soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)"
                                                                     : "";
    findField("note")->setText(
        std::to_string(selectedNoteParameters->getNumber()) + "/" + padName +
        "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void PgmParamsScreen::displayPgm()
{
    findField("pgm")->setTextPadded(activeDrum().getProgram() + 1, " ");
}

void PgmParamsScreen::displayTune()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto tune = selectedNoteParameters->getTune();
    auto sign = tune < 0 ? "-" : " ";
    auto number = StrUtil::padLeft(std::to_string(abs(tune)), " ", 3);
    findField("tune")->setText(sign + number);
}

void PgmParamsScreen::displayDecayMode()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("dcymd")->setText(
        decayModes[selectedNoteParameters->getDecayMode()]);
    displayAttackDecay();
}

void PgmParamsScreen::displayVoiceOverlap()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
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
