#include "EditMultipleScreen.hpp"

#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"

#include "lcdgui/screens/StepEditorScreen.hpp"

#include <Util.hpp>

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;

EditMultipleScreen::EditMultipleScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "edit-multiple", layerIndex)
{
}

void EditMultipleScreen::open()
{
    updateEditMultiple();
    mpc.clientEventController->addObserver(
        this); // Subscribe to "note" messages
}

void EditMultipleScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void EditMultipleScreen::function(int i)
{
    ScreenComponent::function(i);
    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();

    auto selectedEvent = stepEditorScreen->getSelectedEvent();
    auto paramLetter = stepEditorScreen->getSelectedParameterLetter();
    auto track = mpc.getSequencer()->getActiveTrack();

    switch (i)
    {
        case 4:
        {
            auto noteEvent =
                std::dynamic_pointer_cast<NoteOnEvent>(selectedEvent);

            if (noteEvent && track->getBus() != 0)
            {
                if (paramLetter == "a")
                {
                    checkNotes();
                }
                else if (paramLetter == "b")
                {
                    for (auto &event : stepEditorScreen->getSelectedEvents())
                    {
                        auto _noteEvent =
                            std::dynamic_pointer_cast<NoteOnEvent>(event);
                        if (_noteEvent)
                        {
                            _noteEvent->setVariationType(variationType);
                        }
                    }
                }
                else if (paramLetter == "c")
                {
                    for (auto &event : stepEditorScreen->getSelectedEvents())
                    {
                        auto _noteEvent =
                            std::dynamic_pointer_cast<NoteOnEvent>(event);

                        if (_noteEvent)
                        {
                            _noteEvent->setVariationValue(variationValue);
                        }
                    }
                }
                else if (paramLetter == "d")
                {
                    checkFiveParameters();
                }
                else if (paramLetter == "e")
                {
                    checkThreeParameters();
                }
            }

            if (noteEvent && track->getBus() == 0)
            {
                if (paramLetter == "a")
                {
                    checkNotes();
                }
                else if (paramLetter == "b")
                {
                    checkFiveParameters();
                }
                else if (paramLetter == "c")
                {
                    checkThreeParameters();
                }
            }

            if (std::dynamic_pointer_cast<ControlChangeEvent>(selectedEvent))
            {
                if (paramLetter == "a")
                {
                    checkFiveParameters();
                }
                else if (paramLetter == "b")
                {
                    checkThreeParameters();
                }
            }

            if (std::dynamic_pointer_cast<ProgramChangeEvent>(selectedEvent) ||
                std::dynamic_pointer_cast<ChannelPressureEvent>(selectedEvent))
            {
                checkFiveParameters();
            }

            if (std::dynamic_pointer_cast<PolyPressureEvent>(selectedEvent))
            {
                if (paramLetter == "a")
                {
                    checkFiveParameters();
                }
                else if (paramLetter == "b")
                {
                    checkThreeParameters();
                }
            }

            stepEditorScreen->clearSelection();
            openScreenById(ScreenId::StepEditorScreen);
        }
    }
}

void EditMultipleScreen::turnWheel(int i)
{

    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();
    auto event = stepEditorScreen->getSelectedEvent();

    auto paramLetter = stepEditorScreen->getSelectedParameterLetter();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    auto track = mpc.getSequencer()->getActiveTrack();

    if (focusedFieldName == "value0")
    {
        auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);

        if (noteEvent && track->getBus() != 0)
        {
            if (paramLetter == "a")
            {
                setChangeNoteTo(changeNoteTo + i);
            }
            else if (paramLetter == "b")
            {
                incrementVariationType(i);
            }
            else if (paramLetter == "c")
            {
                setVariationValue(variationValue + i);
            }
            else if (paramLetter == "d" || paramLetter == "e")
            {
                setEditType(editType + i);
            }
        }
        else if (noteEvent && track->getBus() == 0)
        {
            if (paramLetter == "a")
            {
                setChangeNoteTo(changeNoteTo + i);
            }
            else if (paramLetter == "b" || paramLetter == "c")
            {
                setEditType(editType + i);
            }
        }
        else if (std::dynamic_pointer_cast<ProgramChangeEvent>(event) ||
                 std::dynamic_pointer_cast<PolyPressureEvent>(event) ||
                 std::dynamic_pointer_cast<ChannelPressureEvent>(event) ||
                 std::dynamic_pointer_cast<ControlChangeEvent>(event))
        {
            setEditType(editType + i);
        }
    }
    else if (focusedFieldName == "value1")
    {
        setEditValue(editValue + i);
    }

    updateEditMultiple();
}

void EditMultipleScreen::checkThreeParameters()
{
    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();

    for (auto &event : stepEditorScreen->getSelectedEvents())
    {

        auto note = std::dynamic_pointer_cast<NoteOnEvent>(event);
        auto controlChange =
            std::dynamic_pointer_cast<ControlChangeEvent>(event);
        auto polyPressure = std::dynamic_pointer_cast<PolyPressureEvent>(event);

        if (note)
        {
            note->setVelocity(editValue);
        }
        else if (controlChange)
        {
            controlChange->setAmount(editValue);
        }
        else if (polyPressure)
        {
            polyPressure->setAmount(editValue);
        }
    }
}

void EditMultipleScreen::checkFiveParameters()
{
    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();

    for (auto &event : stepEditorScreen->getSelectedEvents())
    {
        auto note = std::dynamic_pointer_cast<NoteOnEvent>(event);
        auto programChange =
            std::dynamic_pointer_cast<ProgramChangeEvent>(event);
        auto controlChange =
            std::dynamic_pointer_cast<ControlChangeEvent>(event);
        auto channelPressure =
            std::dynamic_pointer_cast<ChannelPressureEvent>(event);
        auto polyPressure = std::dynamic_pointer_cast<PolyPressureEvent>(event);

        if (note)
        {
            note->setDuration(editValue);
        }
        else if (programChange)
        {
            programChange->setProgram(editValue);
        }
        else if (controlChange)
        {
            controlChange->setController(editValue);
        }
        else if (channelPressure)
        {
            channelPressure->setAmount(editValue);
        }
        else if (polyPressure)
        {
            polyPressure->setNote(editValue);
        }
    }
}

void EditMultipleScreen::checkNotes()
{
    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();
    for (auto &event : stepEditorScreen->getSelectedEvents())
    {
        auto note = std::dynamic_pointer_cast<NoteOnEvent>(event);

        if (note)
        {
            note->setNote(changeNoteTo);
        }
    }
}

void EditMultipleScreen::setEditType(int i)
{
    editType = std::clamp(i, 0, 3);
    setEditValue(editValue); // re-clamp
    updateEditMultiple();
}

void EditMultipleScreen::updateEditMultiple()
{

    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();
    auto event = stepEditorScreen->getSelectedEvent();
    auto letter = stepEditorScreen->getSelectedParameterLetter();

    auto track = mpc.getSequencer()->getActiveTrack();

    if (std::dynamic_pointer_cast<NoteOnEvent>(event) && track->getBus() != 0)
    {
        if (letter == "a" || letter == "b" || letter == "c")
        {
            findLabel("value1")->Hide(true);
            findField("value1")->Hide(true);
            findLabel("value0")->Hide(false);
            findLabel("value0")->setLocation(xPosSingle, yPosSingle);

            if (letter == "a")
            {
                findLabel("value0")->setText(singleLabels[0]);
                findField("value0")->setSize(6 * 6 + 1, 9);

                auto program = getProgramOrThrow();
                auto padName = sampler->getPadName(
                    program->getPadIndexFromNote(changeNoteTo));
                auto noteName =
                    changeNoteTo == 34 ? "--" : std::to_string(changeNoteTo);
                findField("value0")->setText(noteName + "/" + padName);
            }
            else if (letter == "b")
            {
                findLabel("value0")->setText(singleLabels[1]);
                findField("value0")->setSize(3 * 6 + 1, 9);
                findField("value0")->setText(
                    noteVariationParameterNames[variationType]);
            }
            else if (letter == "c")
            {
                findLabel("value0")->setText(singleLabels[2]);

                if (variationType == NoteOnEvent::VARIATION_TYPE::TUNE_0)
                {
                    findField("value0")->setSize(4 * 6 + 1, 9);
                    findField("value0")->setLocation(
                        45, findField("value0")->getY());
                    auto noteVarValue = (variationValue * 2) - 128;

                    if (noteVarValue < -120)
                    {
                        noteVarValue = -120;
                    }
                    else if (noteVarValue > 120)
                    {
                        noteVarValue = 120;
                    }

                    if (noteVarValue == 0)
                    {
                        findField("value0")->setTextPadded(0, " ");
                    }
                    else if (noteVarValue < 0)
                    {
                        findField("value0")->setText(
                            "-" +
                            StrUtil::padLeft(std::to_string(abs(noteVarValue)),
                                             " ", 3));
                    }
                    else
                    {
                        findField("value0")->setText(
                            "+" + StrUtil::padLeft(std::to_string(noteVarValue),
                                                   " ", 3));
                    }
                }

                if (variationType == NoteOnEvent::VARIATION_TYPE::DECAY_1 ||
                    variationType == NoteOnEvent::VARIATION_TYPE::ATTACK_2)
                {
                    auto noteVarValue = variationValue;

                    if (noteVarValue > 100)
                    {
                        noteVarValue = 100;
                    }

                    findField("value0")->setText(
                        StrUtil::padLeft(std::to_string(noteVarValue), " ", 3));
                    findField("value0")->setSize(3 * 6 + 1, 9);
                    findField("value0")->setLocation(
                        51, findField("value0")->getY());
                }
                else if (variationType == NoteOnEvent::VARIATION_TYPE::FILTER_3)
                {
                    findField("value0")->setSize(4 * 6 + 1, 9);
                    findField("value0")->setLocation(
                        45, findField("value0")->getY());

                    auto noteVarValue = variationValue - 50;

                    if (noteVarValue > 50)
                    {
                        noteVarValue = 50;
                    }

                    if (noteVarValue < 0)
                    {
                        findField("value0")->setText(
                            "-" +
                            StrUtil::padLeft(std::to_string(abs(noteVarValue)),
                                             " ", 2));
                    }
                    else if (noteVarValue > 0)
                    {
                        findField("value0")->setText(
                            "+" + StrUtil::padLeft(std::to_string(noteVarValue),
                                                   " ", 2));
                    }
                    else
                    {
                        findField("value0")->setTextPadded("0", " ");
                    }
                }
            }

            findLabel("value0")->setSize(
                findLabel("value0")->getText().length() * 6 + 1, 9);
            findField("value0")->Hide(false);
            findField("value0")->setLocation(
                xPosSingle + findLabel("value0")->getW(), yPosSingle);
        }
        else if (letter == "d" || letter == "e")
        {
            updateDouble();
        }
    }

    if (std::dynamic_pointer_cast<NoteOnEvent>(event) && track->getBus() == 0)
    {
        if (letter == "a")
        {
            findLabel("value1")->Hide(true);
            findField("value1")->Hide(true);
            findLabel("value0")->Hide(false);
            findLabel("value0")->setLocation(xPosSingle, yPosSingle);
            findLabel("value0")->setText(singleLabels[0]);
            findField("value0")->setSize(8 * 6 + 1, 9);
            findField("value0")->setText(
                (StrUtil::padLeft(std::to_string(changeNoteTo), " ", 3) + "(" +
                 mpc::Util::noteNames()[changeNoteTo]) +
                ")");
            findLabel("value0")->setSize(
                findLabel("value0")->GetTextEntryLength() * 6 + 1, 9);
            findField("value0")->Hide(false);
            findField("value0")->setLocation(
                xPosSingle + findLabel("value0")->getW(), yPosSingle);
        }
        else if (letter == "b" || letter == "c")
        {
            updateDouble();
        }
    }
    if (std::dynamic_pointer_cast<ProgramChangeEvent>(event) ||
        std::dynamic_pointer_cast<PolyPressureEvent>(event) ||
        std::dynamic_pointer_cast<ChannelPressureEvent>(event) ||
        std::dynamic_pointer_cast<ControlChangeEvent>(event))
    {
        updateDouble();
    }
}

void EditMultipleScreen::updateDouble()
{
    auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();

    findLabel("value0")->Hide(false);
    findLabel("value1")->Hide(false);
    findField("value0")->Hide(false);
    findField("value1")->Hide(false);
    findLabel("value0")->setText(doubleLabels[0]);
    findLabel("value1")->setText(doubleLabels[1]);
    findLabel("value0")->setSize(
        findLabel("value0")->GetTextEntryLength() * 6 + 1, 9);
    findLabel("value0")->setLocation(xPosDouble[0], yPosDouble[0]);
    findLabel("value1")->setSize(
        findLabel("value1")->GetTextEntryLength() * 6 + 1, 9);
    findLabel("value1")->setLocation(xPosDouble[1], yPosDouble[1]);
    findField("value0")->setLocation(
        (xPosDouble[0] + findLabel("value0")->getW()), yPosDouble[0]);
    findField("value1")->setLocation(
        (xPosDouble[1] + findLabel("value1")->getW()), yPosDouble[1]);
    findField("value0")->setText(editTypeNames[editType]);
    findField("value1")->setText(std::to_string(editValue));
    findField("value0")->setSize(10 * 6 + 1, 9);
    findField("value1")->setSize(3 * 6 + 1, 9);
}

void EditMultipleScreen::setChangeNoteTo(int i)
{
    auto track = mpc.getSequencer()->getActiveTrack();
    auto midi = track->getBus() == 0;
    changeNoteTo = std::clamp(i, midi ? 0 : 34, midi ? 127 : 98);
    updateEditMultiple();
}

void EditMultipleScreen::setVariationType(NoteOnEvent::VARIATION_TYPE type)
{
    variationType = type;
    updateEditMultiple();
}

void mpc::lcdgui::screens::window::EditMultipleScreen::incrementVariationType(
    int i)
{
    variationType =
        mpc::sequencer::NoteOnEvent::VARIATION_TYPE(std::clamp(i, 0, 3));
    updateEditMultiple();
}

void EditMultipleScreen::setVariationValue(int i)
{
    variationValue = std::clamp(
        i, 0, variationType == NoteOnEvent::VARIATION_TYPE::TUNE_0 ? 124 : 100);
    updateEditMultiple();
}

void EditMultipleScreen::setEditValue(int i)
{
    editValue = std::clamp(i, 0, editType == 2 ? 200 : 127);
    updateEditMultiple();
}
