#include "Assign16LevelsScreen.hpp"

#include "Mpc.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Label.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

Assign16LevelsScreen::Assign16LevelsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "assign-16-levels", layerIndex)
{
}

void Assign16LevelsScreen::open()
{
    displayNote();
    displayParameter();
    displayType();
    displayOriginalKeyPad();
}

void Assign16LevelsScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
        {
            mpc.clientEventController->setSixteenLevelsEnabled(true);
            mpc.getHardware()
                ->getLed(hardware::ComponentId::SIXTEEN_LEVELS_OR_SPACE_LED)
                ->setEnabled(true);
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        }
        default:;
    }
}

void Assign16LevelsScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "note")
    {
        DrumNoteNumber candidate = note + i;

        if (candidate < MinDrumNoteNumber)
        {
            candidate = MinDrumNoteNumber;
        }
        else if (candidate > MaxDrumNoteNumber)
        {
            candidate = MaxDrumNoteNumber;
        }

        setNote(candidate);
    }
    else if (focusedFieldName == "param")
    {
        setParam(parameter + i);
    }
    else if (focusedFieldName == "type")
    {
        setType(type + i);
    }
    else if (focusedFieldName == "originalkeypad")
    {
        setOriginalKeyPad(originalKeyPad + i);
    }
}

void Assign16LevelsScreen::setNote(DrumNoteNumber newNote)
{
    if (newNote < NoDrumNoteAssigned)
    {
        newNote = NoDrumNoteAssigned;
    }
    else if (newNote > MaxDrumNoteNumber)
    {
        newNote = MaxDrumNoteNumber;
    }

    if (note == newNote)
    {
        return;
    }

    note = newNote;
    displayNote();
}

void Assign16LevelsScreen::setParam(const int i)
{
    parameter = std::clamp(i, 0, 1);
    displayParameter();
    displayType();
}

void Assign16LevelsScreen::setType(const int i)
{
    type = std::clamp(i, 0, 3);

    displayType();
    displayOriginalKeyPad();
}

void Assign16LevelsScreen::setOriginalKeyPad(const int i)
{
    originalKeyPad = std::clamp(i, 3, 12);
    displayOriginalKeyPad();
}

int Assign16LevelsScreen::getOriginalKeyPad() const
{
    return originalKeyPad;
}

int Assign16LevelsScreen::getType() const
{
    return type;
}

mpc::DrumNoteNumber Assign16LevelsScreen::getNote() const
{
    return note;
}

int Assign16LevelsScreen::getParameter() const
{
    return parameter;
}

void Assign16LevelsScreen::displayNote() const
{
    const auto track = mpc.getSequencer()->getSelectedTrack();
    const auto drumBus = sequencer.lock()->getBus<DrumBus>(track->getBusType());
    assert(drumBus);
    const auto program = sampler.lock()->getProgram(drumBus->getProgramIndex());
    const auto padIndex = program->getPadIndexFromNote(note);

    const auto padName = sampler.lock()->getPadName(padIndex);

    const auto soundIndex =
        note == NoDrumNoteAssigned
            ? -1
            : program->getNoteParameters(note)->getSoundIndex();
    const auto soundName = soundIndex == -1
                               ? "(No sound)"
                               : sampler.lock()->getSoundName(soundIndex);

    const auto noteName =
        note == NoDrumNoteAssigned ? "--" : std::to_string(note);

    findField("note")->setText(noteName + "/" + padName + "-" + soundName);
}

void Assign16LevelsScreen::displayParameter() const
{
    findField("param")->setText(paramNames[parameter]);
}

void Assign16LevelsScreen::displayType() const
{
    displayOriginalKeyPad();

    findField("type")->Hide(parameter != 1);
    findLabel("type")->Hide(parameter != 1);

    if (parameter != 1)
    {
        return;
    }

    findField("type")->setText(typeNames[type]);
}

void Assign16LevelsScreen::displayOriginalKeyPad() const
{
    findField("originalkeypad")->Hide(!(parameter == 1 && type == 0));
    findLabel("originalkeypad")->Hide(!(parameter == 1 && type == 0));

    if (type != 0)
    {
        return;
    }

    findField("originalkeypad")->setTextPadded(originalKeyPad + 1, " ");
}
