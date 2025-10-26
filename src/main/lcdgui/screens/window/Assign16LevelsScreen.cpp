#include "Assign16LevelsScreen.hpp"

#include <Mpc.hpp>

#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"

#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

Assign16LevelsScreen::Assign16LevelsScreen(mpc::Mpc &mpc, const int layerIndex)
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

void Assign16LevelsScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
        {
            mpc.clientEventController->setSixteenLevelsEnabled(true);
            mpc.getHardware()->getLed(hardware::ComponentId::SIXTEEN_LEVELS_OR_SPACE_LED)->setEnabled(true);
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        }
    }
}

void Assign16LevelsScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "note")
    {
        auto candidate = note + i;

        if (candidate < 35)
        {
            candidate = 35;
        }
        else if (candidate > 98)
        {
            candidate = 98;
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

void Assign16LevelsScreen::setNote(int newNote)
{
    if (newNote < 34)
    {
        newNote = 34;
    }
    else if (newNote > 98)
    {
        newNote = 98;
    }

    if (note == newNote)
    {
        return;
    }

    note = newNote;
    displayNote();
}

void Assign16LevelsScreen::setParam(int i)
{
    parameter = std::clamp(i, 0, 1);
    displayParameter();
    displayType();
}

void Assign16LevelsScreen::setType(int i)
{
    type = std::clamp(i, 0, 3);

    displayType();
    displayOriginalKeyPad();
}

void Assign16LevelsScreen::setOriginalKeyPad(int i)
{
    originalKeyPad = std::clamp(i, 3, 12);
    displayOriginalKeyPad();
}

int Assign16LevelsScreen::getOriginalKeyPad()
{
    return originalKeyPad;
}

int Assign16LevelsScreen::getType()
{
    return type;
}

int Assign16LevelsScreen::getNote()
{
    return note;
}

int Assign16LevelsScreen::getParameter()
{
    return parameter;
}

void Assign16LevelsScreen::displayNote()
{
    auto track = mpc.getSequencer()->getActiveTrack();
    auto pgmNumber = sampler->getDrumBusProgramIndex(track->getBus());
    auto program = sampler->getProgram(pgmNumber);
    auto padIndex = program->getPadIndexFromNote(note);

    auto padName = sampler->getPadName(padIndex);

    auto soundIndex = note == 34 ? -1 : program->getNoteParameters(note)->getSoundIndex();
    auto soundName = soundIndex == -1 ? "(No sound)" : sampler->getSoundName(soundIndex);

    auto noteName = note == 34 ? "--" : std::to_string(note);

    findField("note")->setText(noteName + "/" + padName + "-" + soundName);
}

void Assign16LevelsScreen::displayParameter()
{
    findField("param")->setText(paramNames[parameter]);
}

void Assign16LevelsScreen::displayType()
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

void Assign16LevelsScreen::displayOriginalKeyPad()
{
    findField("originalkeypad")->Hide(!(parameter == 1 && type == 0));
    findLabel("originalkeypad")->Hide(!(parameter == 1 && type == 0));

    if (type != 0)
    {
        return;
    }

    findField("originalkeypad")->setTextPadded(originalKeyPad + 1, " ");
}
