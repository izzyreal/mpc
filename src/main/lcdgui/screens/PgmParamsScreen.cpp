#include "PgmParamsScreen.hpp"

#include "SelectDrumScreen.hpp"
#include "sampler/VoiceOverlapMode.hpp"

#include <sampler/NoteParameters.hpp>

#include <lcdgui/EnvGraph.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

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
    init();

    auto lastNoteParameters = sampler->getLastNp(program.get());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tune")
    {
        lastNoteParameters->setTune(lastNoteParameters->getTune() + i);
        displayTune();
    }
    else if (focusedFieldName == "dcymd")
    {
        lastNoteParameters->setDecayMode(lastNoteParameters->getDecayMode() + i);
        displayDecayMode();
    }
    else if (focusedFieldName == "voiceoverlap")
    {
        auto s = sampler->getSound(lastNoteParameters->getSoundIndex());

        if (s && s->isLoopEnabled())
        {
            return;
        }

        using M = sampler::VoiceOverlapMode;

        int modeVal = static_cast<int>(lastNoteParameters->getVoiceOverlapMode());
        modeVal += i;

        modeVal = std::clamp(modeVal, static_cast<int>(M::POLY), static_cast<int>(M::NOTE_OFF));

        lastNoteParameters->setVoiceOverlapMode(static_cast<M>(modeVal));
        
        displayVoiceOverlap();
    }
    else if (focusedFieldName == "reson")
    {
        lastNoteParameters->setFilterResonance(lastNoteParameters->getFilterResonance() + i);
        displayReson();
    }
    else if (focusedFieldName == "freq")
    {
        lastNoteParameters->setFilterFrequency(lastNoteParameters->getFilterFrequency() + i);
        displayFreq();
    }
    else if (focusedFieldName == "decay")
    {
        lastNoteParameters->setDecay(lastNoteParameters->getDecay() + i);
        displayAttackDecay();
    }
    else if (focusedFieldName == "attack")
    {
        lastNoteParameters->setAttack(lastNoteParameters->getAttack() + i);
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
    else if(focusedFieldName == "note")
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

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "pgm")
    {
        mpc.getLayeredScreen()->openScreen<ProgramScreen>();
    }
    else if (focusedFieldName == "note")
    {
        mpc.getLayeredScreen()->openScreen<CopyNoteParametersScreen>();
    }
    else if (focusedFieldName == "attack" || focusedFieldName == "decay" || focusedFieldName == "dcymd")
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

    const auto lastNoteParameters = sampler->getLastNp(program.get());
    sampler::VoiceOverlapMode mode = lastNoteParameters->getVoiceOverlapMode();

    const auto sound = sampler->getSound(lastNoteParameters->getSoundIndex());

    if (sound && sound->isLoopEnabled())
    {
        mode = sampler::VoiceOverlapMode::NOTE_OFF;
    }

    findField("voiceoverlap")->setText(voiceOverlapModes[static_cast<int>(mode)]);
}
