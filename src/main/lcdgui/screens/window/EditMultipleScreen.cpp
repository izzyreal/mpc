#include "EditMultipleScreen.hpp"

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>

#include <lcdgui/screens/StepEditorScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

EditMultipleScreen::EditMultipleScreen(mpc::Mpc& mpc, const int layerIndex)
: ScreenComponent(mpc, "edit-multiple", layerIndex)
{
}

void EditMultipleScreen::open()
{
    updateEditMultiple();
    mpc.addObserver(this); // Subscribe to "note" messages
}

void EditMultipleScreen::close()
{
    mpc.deleteObserver(this);
}

void EditMultipleScreen::function(int i)
{
    ScreenComponent::function(i);
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    
    auto selectedEvent = stepEditorScreen->getSelectedEvent();
    string paramLetter = stepEditorScreen->getSelectedParameterLetter();
    
    switch (i)
    {
        case 4:
        {
            auto noteEvent = dynamic_pointer_cast<NoteEvent>(selectedEvent);
            
            if (noteEvent && track.lock()->getBus() != 0)
            {
                if (paramLetter == "a")
                {
                    checkNotes();
                }
                else if (paramLetter == "b")
                {
                    for (auto& event : stepEditorScreen->getSelectedEvents())
                    {
                        auto _noteEvent = dynamic_pointer_cast<NoteEvent>(event);
                        if (_noteEvent)
                            _noteEvent->setVariationTypeNumber(variationType);
                    }
                }
                else if (paramLetter == "c")
                {
                    for (auto& event : stepEditorScreen->getSelectedEvents())
                    {
                        auto _noteEvent = dynamic_pointer_cast<NoteEvent>(event);

                        if (_noteEvent)
                            _noteEvent->setVariationValue(variationValue);
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
            
            if (noteEvent && track.lock()->getBus() == 0)
            {
                if (paramLetter == "a")
                    checkNotes();
                else if (paramLetter == "b")
                    checkFiveParameters();
                else if (paramLetter == "c")
                    checkThreeParameters();
            }
            
            if (dynamic_pointer_cast<ControlChangeEvent>(selectedEvent))
            {
                if (paramLetter == "a")
                    checkFiveParameters();
                else if (paramLetter == "b")
                    checkThreeParameters();
            }
            
            if (dynamic_pointer_cast<ProgramChangeEvent>(selectedEvent) || dynamic_pointer_cast<ChannelPressureEvent>(selectedEvent))
            {
                checkFiveParameters();
            }
            
            if (dynamic_pointer_cast<PolyPressureEvent>(selectedEvent))
            {
                if (paramLetter == "a")
                    checkFiveParameters();
                else if (paramLetter == "b")
                    checkThreeParameters();
            }
            
            stepEditorScreen->clearSelection();
            openScreen("step-editor");
        }
    }
}

void EditMultipleScreen::turnWheel(int i)
{
    init();
    
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    auto event = stepEditorScreen->getSelectedEvent();
    
    string paramLetter = stepEditorScreen->getSelectedParameterLetter();
    
    if (param == "value0")
    {
        auto noteEvent = dynamic_pointer_cast<NoteEvent>(event);
        
        if (noteEvent && track.lock()->getBus() != 0)
        {
            if (paramLetter == "a")
            {
                if (changeNoteTo == 34 && i < 0)
                {
                    return;
                }

                auto candidate = changeNoteTo + i;

                if (candidate < 35)
                {
                    candidate = 35;
                }
                else if (candidate > 98)
                {
                    candidate = 98;
                }

                setChangeNoteTo(candidate);
            }
            else if (paramLetter == "b")
            {
                setVariationType(variationType + i);
            }
            else if (paramLetter == "c")
            {
                seVariationValue(variationValue + i);
            }
            else if (paramLetter == "d" || paramLetter == "e")
            {
                setEditType(editType + i);
            }
        }
        else if (noteEvent && track.lock()->getBus() == 0)
        {
            if (paramLetter == "a")
                setChangeNoteTo(changeNoteTo + i);
            else if (paramLetter == "b" || paramLetter == "c")
                setEditType(editType + i);
        }
        else if (dynamic_pointer_cast<ProgramChangeEvent>(event)
                 || dynamic_pointer_cast<PolyPressureEvent>(event)
                 || dynamic_pointer_cast<ChannelPressureEvent>(event)
                 || dynamic_pointer_cast<ControlChangeEvent>(event))
        {
            setEditType(editType + i);
        }
    }
    else if (param == "value1")
    {
        setEditValue(editValue + i);
    }
    
    updateEditMultiple();
}

void EditMultipleScreen::checkThreeParameters()
{
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    
    for (auto& event : stepEditorScreen->getSelectedEvents())
    {
        
        auto note = dynamic_pointer_cast<NoteEvent>(event);
        auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(event);
        auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(event);
        
        if (note)
            note->setVelocity(editValue);
        else if (controlChange)
            controlChange->setAmount(editValue);
        else if (polyPressure)
            polyPressure->setAmount(editValue);
    }
}

void EditMultipleScreen::checkFiveParameters()
{
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    
    for (auto& event : stepEditorScreen->getSelectedEvents())
    {
        auto note = dynamic_pointer_cast<NoteEvent>(event);
        auto programChange = dynamic_pointer_cast<ProgramChangeEvent>(event);
        auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(event);
        auto channelPressure = dynamic_pointer_cast<ChannelPressureEvent>(event);
        auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(event);
        
        if (note)
            note->setDuration(editValue);
        else if (programChange)
            programChange->setProgram(editValue);
        else if (controlChange)
            controlChange->setController(editValue);
        else if (channelPressure)
            channelPressure->setAmount(editValue);
        else if (polyPressure)
            polyPressure->setNote(editValue);
    }
}

void EditMultipleScreen::checkNotes()
{
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    for (auto& event : stepEditorScreen->getSelectedEvents())
    {
        auto note = dynamic_pointer_cast<NoteEvent>(event);
        
        if (note)
            note->setNote(changeNoteTo);
    }
}

void EditMultipleScreen::setEditType(int i)
{
    if (i < 0 || i > 3)
        return;
    
    editType = i;
    
    if (editType != 2 && editValue > 127)
        editValue = 127;
    
    updateEditMultiple();
}

void EditMultipleScreen::updateEditMultiple()
{
    init();
    
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    auto event = stepEditorScreen->getSelectedEvent();
    auto letter = stepEditorScreen->getSelectedParameterLetter();
    
    if (dynamic_pointer_cast<NoteEvent>(event) && track.lock()->getBus() != 0)
    {
        if (letter == "a" || letter == "b" || letter == "c")
        {
            findLabel("value1").lock()->Hide(true);
            findField("value1").lock()->Hide(true);
            findLabel("value0").lock()->Hide(false);
            findLabel("value0").lock()->setLocation(xPosSingle, yPosSingle);
            
            if (letter == "a")
            {
                findLabel("value0").lock()->setText(singleLabels[0]);
                findField("value0").lock()->setSize(6 * 6 + 1, 9);

                auto padName = sampler->getPadName(program.lock()->getPadIndexFromNote(changeNoteTo));
                auto noteName = changeNoteTo == 34 ? "--" : std::to_string(changeNoteTo);
                findField("value0").lock()->setText(noteName + "/" + padName);
            }
            else if (letter == "b")
            {
                findLabel("value0").lock()->setText(singleLabels[1]);
                findField("value0").lock()->setSize(3 * 6 + 1, 9);
                findField("value0").lock()->setText(noteVariationParameterNames[variationType]);
            }
            else if (letter == "c")
            {
                findLabel("value0").lock()->setText(singleLabels[2]);
                
                if (variationType == 0)
                {
                    findField("value0").lock()->setSize(4 * 6 + 1, 9);
                    findField("value0").lock()->setLocation(45, findField("value0").lock()->getY());
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
                        findField("value0").lock()->setTextPadded(0, " ");
                    }
                    else if (noteVarValue < 0)
                    {
                        findField("value0").lock()->setText("-" + StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 3));
                    }
                    else
                    {
                        findField("value0").lock()->setText("+" + StrUtil::padLeft(to_string(noteVarValue), " ", 3));
                    }
                }
                
                if (variationType == 1 || variationType == 2)
                {
                    auto noteVarValue = variationValue;
                    
                    if (noteVarValue > 100)
                        noteVarValue = 100;
                    
                    findField("value0").lock()->setText(StrUtil::padLeft(to_string(noteVarValue), " ", 3));
                    findField("value0").lock()->setSize(3 * 6 + 1, 9);
                    findField("value0").lock()->setLocation(51, findField("value0").lock()->getY());
                }
                else if (variationType == 3)
                {
                    findField("value0").lock()->setSize(4 * 6 + 1, 9);
                    findField("value0").lock()->setLocation(45, findField("value0").lock()->getY());
                    
                    auto noteVarValue = variationValue - 50;
                    
                    if (noteVarValue > 50)
                        noteVarValue = 50;
                    
                    if (noteVarValue < 0)
                        findField("value0").lock()->setText("-" + StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 2));
                    else if (noteVarValue > 0)
                        findField("value0").lock()->setText("+" + StrUtil::padLeft(to_string(noteVarValue), " ", 2));
                    else
                        findField("value0").lock()->setTextPadded("0", " ");
                }
            }
            
            findLabel("value0").lock()->setSize(findLabel("value0").lock()->getText().length() * 6 + 1, 9);
            findField("value0").lock()->Hide(false);
            findField("value0").lock()->setLocation(xPosSingle + findLabel("value0").lock()->getW(), yPosSingle);
        }
        else if (letter == "d" || letter == "e")
        {
            updateDouble();
        }
    }
    
    if (dynamic_pointer_cast<NoteEvent>(event) && track.lock()->getBus() == 0)
    {
        if (letter == "a")
        {
            findLabel("value1").lock()->Hide(true);
            findField("value1").lock()->Hide(true);
            findLabel("value0").lock()->Hide(false);
            findLabel("value0").lock()->setLocation(xPosSingle, yPosSingle);
            findLabel("value0").lock()->setText(singleLabels[0]);
            findField("value0").lock()->setSize(8 * 6 + 1, 9);
            findField("value0").lock()->setText((StrUtil::padLeft(to_string(changeNoteTo), " ", 3) + "(" + mpc::Util::noteNames()[changeNoteTo]) + ")");
            findLabel("value0").lock()->setSize(findLabel("value0").lock()->GetTextEntryLength() * 6 + 1, 9);
            findField("value0").lock()->Hide(false);
            findField("value0").lock()->setLocation(xPosSingle + findLabel("value0").lock()->getW(), yPosSingle);
        }
        else if (letter == "b" || letter == "c")
        {
            updateDouble();
        }
    }
    if (dynamic_pointer_cast<ProgramChangeEvent>(event)
        || dynamic_pointer_cast<PolyPressureEvent>(event)
        || dynamic_pointer_cast<ChannelPressureEvent>(event)
        || dynamic_pointer_cast<ControlChangeEvent>(event))
    {
        updateDouble();
    }
}

void EditMultipleScreen::updateDouble()
{
    auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
    
    findLabel("value0").lock()->Hide(false);
    findLabel("value1").lock()->Hide(false);
    findField("value0").lock()->Hide(false);
    findField("value1").lock()->Hide(false);
    findLabel("value0").lock()->setText(doubleLabels[0]);
    findLabel("value1").lock()->setText(doubleLabels[1]);
    findLabel("value0").lock()->setSize(findLabel("value0").lock()->GetTextEntryLength() * 6 + 1, 9);
    findLabel("value0").lock()->setLocation(xPosDouble[0], yPosDouble[0]);
    findLabel("value1").lock()->setSize(findLabel("value1").lock()->GetTextEntryLength() * 6 + 1, 9);
    findLabel("value1").lock()->setLocation(xPosDouble[1], yPosDouble[1]);
    findField("value0").lock()->setLocation((xPosDouble[0] + findLabel("value0").lock()->getW()), yPosDouble[0]);
    findField("value1").lock()->setLocation((xPosDouble[1] + findLabel("value1").lock()->getW()), yPosDouble[1]);
    findField("value0").lock()->setText(editTypeNames[editType]);
    findField("value1").lock()->setText(to_string(editValue));
    findField("value0").lock()->setSize(10 * 6 + 1, 9);
    findField("value1").lock()->setSize(3 * 6 + 1, 9);
}

void EditMultipleScreen::setChangeNoteTo(int i)
{
    init();
    
    auto midi = track.lock()->getBus() == 0;
    
    if (midi)
    {
        if (i < 0 || i > 127)
            return;
    }
    else
    {
        if (i < 34 || i > 98)
            return;
    }
    
    changeNoteTo = i;
    updateEditMultiple();
}

void EditMultipleScreen::setVariationType(int i)
{
    if (i < 0 || i > 3)
        return;
    
    variationType = i;
    updateEditMultiple();
}

void EditMultipleScreen::seVariationValue(int i)
{
    if (i < 0 || i > 128)
        return;
    
    if (variationType != 0 && i > 100)
        i = 100;
    
    variationValue = i;
    updateEditMultiple();
}

void EditMultipleScreen::setEditValue(int i)
{
    if (i < 0)
        return;
    
    if (editType != 2 && i > 127)
        return;
    
    if (editType == 2 && i > 200)
        return;
    
    editValue = i;
    updateEditMultiple();
}
