#include "CopyNoteParametersScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;

CopyNoteParametersScreen::CopyNoteParametersScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "copy-note-parameters", layerIndex)
{
}

void CopyNoteParametersScreen::open()
{
    const auto programIndex = getActiveDrumBus()->getProgramIndex();
    setProg0(programIndex);
    setNote0(mpc.clientEventController->getSelectedNote());
    setProg1(programIndex);
    setNote1(mpc.clientEventController->getSelectedNote() - 35);
}

void CopyNoteParametersScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "prog0")
    {
        const bool up = increment > 0;
        auto candidate = prog0;
        for (int i = 0; i < std::abs(increment); ++i)
        {
            candidate = sampler.lock()->getUsedProgram(candidate, up);
        }
        setProg0(candidate);
    }
    else if (focusedFieldName == "note0")
    {
        setNote0(mpc.clientEventController->getSelectedNote() + increment);
    }
    else if (focusedFieldName == "prog1")
    {
        const bool up = increment > 0;
        auto candidate = prog1;
        for (int i = 0; i < std::abs(increment); ++i)
        {
            candidate = sampler.lock()->getUsedProgram(candidate, up);
        }
        setProg1(candidate);
    }
    else if (focusedFieldName == "note1")
    {
        setNote1(note1 + increment);
    }
}

void CopyNoteParametersScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
        {
            const auto source = sampler.lock()->getProgram(prog0);
            const auto dest = sampler.lock()->getProgram(prog1).get();
            source->cloneNoteParameters(
                mpc.clientEventController->getSelectedNote(), dest,
                DrumNoteNumber(note1 + MinDrumNoteNumber));
            openScreenById(ScreenId::PgmAssignScreen);
            break;
        }
        default:;
    }
}

void CopyNoteParametersScreen::displayProg0() const
{
    const auto program = sampler.lock()->getProgram(prog0);
    findField("prog0")->setText(
        StrUtil::padLeft(std::to_string(prog0 + 1), " ", 2) + "-" +
        program->getName());
}

void CopyNoteParametersScreen::displayNote0() const
{
    const auto sourceProgram = sampler.lock()->getProgram(prog0);
    const auto note0 = mpc.clientEventController->getSelectedNote();
    const auto selectedNoteParameters = sourceProgram->getNoteParameters(note0);
    const auto destProgram = sampler.lock()->getProgram(prog0);
    const auto padIndex = destProgram->getPadIndexFromNote(note0);
    const auto soundIndex = note0 != NoDrumNoteAssigned
                                ? selectedNoteParameters->getSoundIndex()
                                : -1;
    const auto noteText =
        note0 == NoDrumNoteAssigned ? "--" : std::to_string(note0);
    const auto padName = sampler.lock()->getPadName(padIndex);
    auto sampleName = soundIndex != -1
                          ? "-" + sampler.lock()->getSoundName(soundIndex)
                          : "-OFF";

    if (note0 == NoDrumNoteAssigned)
    {
        sampleName = "";
    }

    findField("note0")->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::displayProg1() const
{
    const auto program = sampler.lock()->getProgram(prog1);
    findField("prog1")->setText(
        StrUtil::padLeft(std::to_string(prog1 + 1), " ", 2) + "-" +
        program->getName());
}

void CopyNoteParametersScreen::displayNote1() const
{
    const auto program = sampler.lock()->getProgram(prog1);
    const auto padIndex =
        program->getPadIndexFromNote(DrumNoteNumber(note1 + MinDrumNoteNumber));
    const auto soundIndex =
        note1 != -1 ? program->getNoteParameters(note1 + MinDrumNoteNumber)
                          ->getSoundIndex()
                    : -1;
    const auto noteText =
        note1 == -1 ? "--" : std::to_string(note1 + MinDrumNoteNumber);
    const auto padName = sampler.lock()->getPadName(padIndex);
    auto sampleName = soundIndex != -1
                          ? "-" + sampler.lock()->getSoundName(soundIndex)
                          : "-OFF";

    if (note1 == -1)
    {
        sampleName = "";
    }

    findField("note1")->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::setProg0(const ProgramIndex programIndex)
{
    prog0 = std::clamp(programIndex, MinProgramIndex, MaxProgramIndex);
    displayProg0();
}

void CopyNoteParametersScreen::setProg1(const ProgramIndex programIndex)
{
    prog1 = std::clamp(programIndex, MinProgramIndex, MaxProgramIndex);
    displayProg1();
}

void CopyNoteParametersScreen::setNote0(
    const DrumNoteNumber drumNoteNumber) const
{
    mpc.clientEventController->setSelectedNote(drumNoteNumber);
    displayNote0();
}

void CopyNoteParametersScreen::setNote1(const int i)
{
    note1 = std::clamp(i, 0, 63);
    displayNote1();
}
