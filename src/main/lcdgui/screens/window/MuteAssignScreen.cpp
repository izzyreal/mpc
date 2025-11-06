#include "MuteAssignScreen.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

MuteAssignScreen::MuteAssignScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "mute-assign", layerIndex)
{
}

void MuteAssignScreen::open()
{
    displayNote();
    displayNote0();
    displayNote1();
    mpc.clientEventController->addObserver(
        this); // Subscribe to "note" messages
}

void MuteAssignScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void MuteAssignScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(
            mpc.clientEventController->getSelectedNote() + i);
    }
    else if (focusedFieldName == "note0")
    {
        selectedNoteParameters->setMuteAssignA(
            selectedNoteParameters->getMuteAssignA() + i);
        displayNote0();
    }
    else if (focusedFieldName == "note1")
    {
        selectedNoteParameters->setMuteAssignB(
            selectedNoteParameters->getMuteAssignB() + i);
        displayNote1();
    }
}

void MuteAssignScreen::displayNote() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNote = mpc.clientEventController->getSelectedNote();
    const auto pad = program->getPadIndexFromNote(selectedNote);
    std::string soundName = "OFF";

    const auto padName = sampler->getPadName(pad);
    const auto sound =
        program->getNoteParameters(selectedNote)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note")->setText(std::to_string(selectedNote) + "/" + padName +
                               "-" + soundName);
}

void MuteAssignScreen::displayNote0() const
{
    const auto program = getProgramOrThrow();

    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto note0 = selectedNoteParameters->getMuteAssignA();

    if (note0 == 34)
    {
        findField("note0")->setText("--");
        return;
    }

    const auto pad = program->getPadIndexFromNote(note0);
    std::string soundName = "OFF";
    const auto sound = program->getNoteParameters(note0)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note0")->setText(std::to_string(note0) + "/" +
                                sampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::displayNote1() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto note1 = selectedNoteParameters->getMuteAssignB();

    if (note1 == 34)
    {
        findField("note1")->setText("--");
        return;
    }

    const auto pad = program->getPadIndexFromNote(note1);
    std::string soundName = "OFF";
    const auto sound = program->getNoteParameters(note1)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note1")->setText(std::to_string(note1) + "/" +
                                sampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::update(Observable *o, const Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        displayNote();
        displayNote0();
        displayNote1();
    }
}
