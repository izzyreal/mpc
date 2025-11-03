#include "MuteAssignScreen.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

MuteAssignScreen::MuteAssignScreen(mpc::Mpc &mpc, const int layerIndex)
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

void MuteAssignScreen::turnWheel(int i)
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
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

void MuteAssignScreen::displayNote()
{
    auto program = getProgramOrThrow();
    auto selectedNote = mpc.clientEventController->getSelectedNote();
    auto pad = program->getPadIndexFromNote(selectedNote);
    std::string soundName = "OFF";

    auto padName = sampler->getPadName(pad);
    auto sound = program->getNoteParameters(selectedNote)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note")->setText(std::to_string(selectedNote) + "/" + padName +
                               "-" + soundName);
}

void MuteAssignScreen::displayNote0()
{
    auto program = getProgramOrThrow();

    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto note0 = selectedNoteParameters->getMuteAssignA();

    if (note0 == 34)
    {
        findField("note0")->setText("--");
        return;
    }

    auto pad = program->getPadIndexFromNote(note0);
    std::string soundName = "OFF";
    auto sound = program->getNoteParameters(note0)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note0")->setText(std::to_string(note0) + "/" +
                                sampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::displayNote1()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto note1 = selectedNoteParameters->getMuteAssignB();

    if (note1 == 34)
    {
        findField("note1")->setText("--");
        return;
    }

    auto pad = program->getPadIndexFromNote(note1);
    std::string soundName = "OFF";
    auto sound = program->getNoteParameters(note1)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note1")->setText(std::to_string(note1) + "/" +
                                sampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::update(Observable *o, Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        displayNote();
        displayNote0();
        displayNote1();
    }
}
