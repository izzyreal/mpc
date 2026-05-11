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
        auto getDrumNoteToEditParameters = [this]
        {
            return getProgramOrThrow()->getNoteParameters(drumNoteToEdit);
        };

        addReactiveBinding(
            {[this]
             {
                 return mpc.clientEventController->getSelectedNote();
             },
             [this, getDrumNoteToEditParameters](const auto note)
             {
                 if (const auto focus = getFocusedFieldNameOrThrow();
                     focus == "note")
                 {
                     drumNoteToEdit =
                         std::clamp(note, MinDrumNoteNumber, MaxDrumNoteNumber);
                     displayNote();
                 }
                 else if (focus == "note0")
                 {
                     getDrumNoteToEditParameters()->setMuteAssignA(note);
                 }
                 else if (focus == "note1")
                 {
                     getDrumNoteToEditParameters()->setMuteAssignB(note);
                 }
             }});

        addReactiveBinding(
            {[getDrumNoteToEditParameters]
             {
                 return getDrumNoteToEditParameters()->getMuteAssignA();
             },
             [this](auto)
             {
                 displayNote0();
             }});

        addReactiveBinding(
            {[getDrumNoteToEditParameters]
             {
                 return getDrumNoteToEditParameters()->getMuteAssignB();
             },
             [this](auto)
             {
                 displayNote1();
             }});
    }

    drumNoteToEdit = mpc.clientEventController->getSelectedNote();

    displayNote();
    displayNote0();
    displayNote1();
}

void MuteAssignScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(drumNoteToEdit);

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(drumNoteToEdit + i);
    }
    else if (focusedFieldName == "note0")
    {
        noteParameters->setMuteAssignA(noteParameters->getMuteAssignA() + i);
    }
    else if (focusedFieldName == "note1")
    {
        noteParameters->setMuteAssignB(noteParameters->getMuteAssignB() + i);
    }
}

void MuteAssignScreen::displayNote() const
{
    const auto program = getProgramOrThrow();

    const auto pad = program->getPadIndexFromNote(drumNoteToEdit);
    std::string soundName = "OFF";

    const auto padName = sampler.lock()->getPadName(pad);
    const auto sound =
        program->getNoteParameters(drumNoteToEdit)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler.lock()->getSoundName(sound);
    }

    findField("note")->setText(std::to_string(drumNoteToEdit) + "/" + padName +
                               "-" + soundName);
}

void MuteAssignScreen::displayNote0() const
{
    const auto program = getProgramOrThrow();

    const auto noteParameters = program->getNoteParameters(drumNoteToEdit);
    const auto note0 = noteParameters->getMuteAssignA();

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
        soundName = sampler.lock()->getSoundName(sound);
    }

    findField("note0")->setText(std::to_string(note0) + "/" +
                                sampler.lock()->getPadName(pad) + "-" +
                                soundName);
}

void MuteAssignScreen::displayNote1() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(drumNoteToEdit);
    const auto note1 = noteParameters->getMuteAssignB();

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
        soundName = sampler.lock()->getSoundName(sound);
    }

    findField("note1")->setText(std::to_string(note1) + "/" +
                                sampler.lock()->getPadName(pad) + "-" +
                                soundName);
}
