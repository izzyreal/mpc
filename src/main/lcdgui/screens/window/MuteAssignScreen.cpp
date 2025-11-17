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
    if (isReactiveBindingsEmpty())
    {
        auto getSelectedNote = [this]
        {
            return this->mpc.clientEventController->getSelectedNote();
        };

        auto getSelectedNoteParameters = [this, getSelectedNote]
        {
            return getProgramOrThrow()->getNoteParameters(getSelectedNote());
        };

        addReactiveBinding({[getSelectedNote]
                            {
                                return getSelectedNote();
                            },
                            [this](auto)
                            {
                                displayNote();
                                displayNote0();
                                displayNote1();
                            }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getMuteAssignA();
             },
             [this](auto)
             {
                 displayNote0();
             }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getMuteAssignB();
             },
             [this](auto)
             {
                 displayNote1();
             }});
    }
    displayNote();
    displayNote0();
    displayNote1();
}

void MuteAssignScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(
            mpc.clientEventController->getSelectedNote() + i);
    }
    else if (focusedFieldName == "note0")
    {
        selectedNoteParameters->setMuteAssignA(
            selectedNoteParameters->getMuteAssignA() + i);
    }
    else if (focusedFieldName == "note1")
    {
        selectedNoteParameters->setMuteAssignB(
            selectedNoteParameters->getMuteAssignB() + i);
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

    if (note0 == NoDrumNoteAssigned)
    {
        findField("note0")->setText("--");
        return;
    }

    const auto pad = program->getPadIndexFromNote(note0);
    std::string soundName = "OFF";

    if (const auto sound = program->getNoteParameters(note0)->getSoundIndex();
        sound != -1)
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

    if (note1 == NoDrumNoteAssigned)
    {
        findField("note1")->setText("--");
        return;
    }

    const auto pad = program->getPadIndexFromNote(note1);
    std::string soundName = "OFF";

    if (const auto sound = program->getNoteParameters(note1)->getSoundIndex();
        sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

    findField("note1")->setText(std::to_string(note1) + "/" +
                                sampler->getPadName(pad) + "-" + soundName);
}
