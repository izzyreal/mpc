#include "CopyNoteParametersScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;

CopyNoteParametersScreen::CopyNoteParametersScreen(mpc::Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "copy-note-parameters", layerIndex)
{
}

void CopyNoteParametersScreen::open()
{
    auto program = getProgramOrThrow();
    auto programIndex = getActiveDrumBus()->getProgram();
    setProg0(programIndex);
    setNote0(mpc.clientEventController->getSelectedNote());
    setProg1(programIndex);
    setNote1(mpc.clientEventController->getSelectedNote() - 35);
}

void CopyNoteParametersScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "prog0")
    {
        setProg0(prog0 + i);
    }
    else if (focusedFieldName == "note0")
    {
        setNote0(mpc.clientEventController->getSelectedNote() + i);
    }
    else if (focusedFieldName == "prog1")
    {
        setProg1(prog1 + i);
    }
    else if (focusedFieldName == "note1")
    {
        setNote1(note1 + i);
    }
}

void CopyNoteParametersScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
        {
            auto source = sampler->getProgram(prog0)->getNoteParameters(
                mpc.clientEventController->getSelectedNote());
            auto dest = sampler->getProgram(prog1);
            auto clone = source->clone(note1);
            dest->setNoteParameters(note1, clone);
            openScreenById(ScreenId::PgmAssignScreen);
            break;
        }
    }
}

void CopyNoteParametersScreen::displayProg0()
{
    auto program = sampler->getProgram(prog0);
    findField("prog0")->setText(
        StrUtil::padLeft(std::to_string(prog0 + 1), " ", 2) + "-" +
        program->getName());
}

void CopyNoteParametersScreen::displayNote0()
{
    auto sourceProgram = sampler->getProgram(prog0);
    auto note0 = mpc.clientEventController->getSelectedNote();
    auto selectedNoteParameters = sourceProgram->getNoteParameters(note0);
    auto destProgram = sampler->getProgram(prog0);
    auto padIndex = destProgram->getPadIndexFromNote(note0);
    auto soundIndex =
        note0 != -1 ? selectedNoteParameters->getSoundIndex() : -1;
    auto noteText = note0 == -1 ? "--" : std::to_string(note0);
    auto padName = sampler->getPadName(padIndex);
    auto sampleName =
        soundIndex != -1 ? "-" + sampler->getSoundName(soundIndex) : "-OFF";

    if (note0 == -1)
    {
        sampleName = "";
    }

    findField("note0")->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::displayProg1()
{
    auto program = sampler->getProgram(prog1);
    findField("prog1")->setText(
        StrUtil::padLeft(std::to_string(prog1 + 1), " ", 2) + "-" +
        program->getName());
}

void CopyNoteParametersScreen::displayNote1()
{
    auto program = sampler->getProgram(prog1);
    auto padIndex = program->getPadIndexFromNote(note1 + 35);
    auto soundIndex =
        note1 != -1 ? program->getNoteParameters(note1 + 35)->getSoundIndex()
                    : -1;
    auto noteText = note1 == -1 ? "--" : std::to_string(note1 + 35);
    auto padName = sampler->getPadName(padIndex);
    auto sampleName =
        soundIndex != -1 ? "-" + sampler->getSoundName(soundIndex) : "-OFF";

    if (note1 == -1)
    {
        sampleName = "";
    }

    findField("note1")->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::setProg0(int i)
{
    prog0 = std::clamp(i, 0, sampler->getProgramCount());
    displayProg0();
}

void CopyNoteParametersScreen::setProg1(int i)
{
    prog1 = std::clamp(i, 0, sampler->getProgramCount());
    displayProg1();
}

void CopyNoteParametersScreen::setNote0(int i)
{
    mpc.clientEventController->setSelectedNote(i);
    displayNote0();
}

void CopyNoteParametersScreen::setNote1(int i)
{
    note1 = std::clamp(i, 0, 63);
    displayNote1();
}
