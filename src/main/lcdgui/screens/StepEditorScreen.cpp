#include "StepEditorScreen.hpp"
#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/InsertEventScreen.hpp"

#include "audiomidi/EventHandler.hpp"

#include "hardware/Hardware.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/StepTcScreen.hpp"
#include "lcdgui/screens/window/EditMultipleScreen.hpp"
#include "lcdgui/screens/window/PasteEventScreen.hpp"

#include "lcdgui/Rectangle.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/EventEquals.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/EventRow.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

const int EVENT_ROW_COUNT = 4;

StepEditorScreen::StepEditorScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "step-editor", layerIndex)
{
    lastColumn["empty"] = "a";
    lastColumn["channel-pressure"] = "a";
    lastColumn["control-change"] = "a";
    lastColumn["midi-clock"] = "a";
    lastColumn["mixer"] = "a";
    lastColumn["note-on"] = "a";
    lastColumn["pitch-bend"] = "a";
    lastColumn["poly-pressure"] = "a";
    lastColumn["program-change"] = "a";
    lastColumn["system-exclusive"] = "a";
    lastColumn["tempo-change"] = "a";

    for (int i = 0; i < EVENT_ROW_COUNT; i++)
    {
        addChildT<EventRow>(mpc, i);
    }

    MRECT r(31, 0, 164, 9);
    addChildT<Rectangle>("view-background", r);

    addReactiveBinding(
        {[&]
         {
             auto original = computeVisibleEvents();
             std::vector<std::shared_ptr<mpc::sequencer::Event>> clones;
             clones.reserve(original.size());
             for (auto &e : original)
             {
                 clones.push_back(cloneEvent(e));
             }
             return clones;
         },
         [&](auto)
         {
             refreshEventRows();
         },
         [&](const auto &a, const auto &b)
         {
             return visibleEventsEqual(a, b);
         }});
}

bool StepEditorScreen::visibleEventsEqual(
    const std::vector<std::shared_ptr<mpc::sequencer::Event>> &a,
    const std::vector<std::shared_ptr<mpc::sequencer::Event>> &b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i)
    {
        if ((!a[i] && b[i]) || (a[i] && !b[i]))
        {
            return false;
        }

        if (!a[i] && !b[i])
        {
            continue;
        }

        if (!eventsEqual(a[i], b[i]))
        {
            return false;
        }
    }

    return true;
}

void StepEditorScreen::openWindow()
{
    openScreenById(ScreenId::StepEditOptionsScreen);
}

void StepEditorScreen::open()
{
    sequencer->storeActiveSequenceInUndoPlaceHolder();

    findField("tonote")->setLocation(115, 0);
    findLabel("fromnote")->Hide(true);

    lastRow = 0;

    auto track = sequencer->getActiveTrack();

    if (track->getBus() != 0)
    {
        findField("fromnote")->setAlignment(Alignment::None);
    }
    else
    {
        findField("fromnote")->setAlignment(Alignment::Centered, 18);
        findField("tonote")->setAlignment(Alignment::Centered, 18);

        // We're in a MIDI track, and MIDI notes have only 3 columns.
        if (lastColumn["note-on"] == "d" || lastColumn["note-on"] == "e")
        {
            lastColumn["note-on"] = "c";
        }
    }

    updateComponents();
    setViewNotesText();
    displayView();

    findField("now0")->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
    findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
    findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");

    const auto eventsAtCurrentTick = computeEventsAtCurrentTick();

    if (ls->isPreviousScreen({ScreenId::InsertEventScreen}))
    {
        auto insertEventScreen =
            mpc.screens->get<ScreenId::InsertEventScreen>();

        if (insertEventScreen->isEventAddedBeforeLeavingTheScreen())
        {
            auto eventCount = static_cast<int>(eventsAtCurrentTick.size()) - 1;
            auto event = eventsAtCurrentTick[eventCount - 1];
            auto eventType = event->getTypeName();

            if (eventCount > 4)
            {
                ls->setFocus(lastColumn[eventType] + "3");
                setyOffset(eventCount - 4);
            }
            else
            {
                auto row = eventCount - 1;
                ls->setFocus(lastColumn[eventType] + std::to_string(row));
            }
        }
    }

    const auto visibleEvents = computeVisibleEvents(eventsAtCurrentTick);

    if (ls->isPreviousScreenNot(
            {ScreenId::StepTcScreen, ScreenId::InsertEventScreen,
             ScreenId::PasteEventScreen, ScreenId::EditMultipleScreen}))
    {
        auto eventType = visibleEvents[0]->getTypeName();
        ls->setFocus(lastColumn[eventType] + "0");
    }

    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::close()
{
    auto track = sequencer->getActiveTrack();

    storeColumnForEventAtActiveRow();

    auto nextScreen = ls->getCurrentScreenName();

    if (nextScreen != "step-timing-correct" && nextScreen != "insert-event" &&
        nextScreen != "paste-event" && nextScreen != "edit-multiple")
    {
        track->removeDoubles();
        sequencer->resetUndo();
    }

    clearSelection();
}

void StepEditorScreen::function(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    switch (i)
    {
        case 0:
            openScreenById(ScreenId::StepTcScreen);
            break;
        case 1:
            if (selectionStartIndex != -1)
            {
                // CopySelectedNotes
                setSelectedEvents();
                placeHolder = selectedEvents;
                clearSelection();
            }
            else if (selectionStartIndex == -1 &&
                     focusedFieldName.length() == 2)
            {
                // CopySelectedNote
                auto eventIndex = getActiveRow();

                const auto visibleEvents = computeVisibleEvents();

                auto maybeEmptyEvent = std::dynamic_pointer_cast<EmptyEvent>(
                    visibleEvents[eventIndex]);

                if (!maybeEmptyEvent)
                {
                    placeHolder = {visibleEvents[eventIndex]};
                }
            }
            break;
        case 2:
        {
            if (focusedFieldName.length() != 2)
            {
                return;
            }

            const auto rowIndex = getActiveRow();
            const auto srcLetter = getActiveColumn();

            const auto visibleEvents = computeVisibleEvents();
            const std::string eventType =
                visibleEvents[rowIndex]->getTypeName();
            lastColumn[eventType] = srcLetter;

            if (selectionStartIndex != -1)
            {
                removeEvents();
                ls->setFocus("a0");
                return;
            }

            if (!std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[rowIndex]))
            {
                auto track = sequencer->getActiveTrack();
                for (int e = 0; e < track->getEvents().size(); e++)
                {
                    if (track->getEvents()[e] == visibleEvents[rowIndex])
                    {
                        track->removeEvent(e);
                        break;
                    }
                }

                if (rowIndex == 2 && yOffset > 0)
                {
                    yOffset--;
                }
            }

            refreshEventRows();
            refreshSelection();

            const std::string newEventType =
                visibleEvents[rowIndex]->getTypeName();

            ls->setFocus(lastColumn[newEventType] + std::to_string(rowIndex));
            break;
        }
        case 3:
        {
            bool posIsLastTick = sequencer->getTickPosition() ==
                                 sequencer->getActiveSequence()->getLastTick();

            if (selectionEndIndex == -1)
            {
                if (!posIsLastTick)
                {
                    openScreenById(ScreenId::InsertEventScreen);
                }
            }
            else
            {
                const auto row = getActiveRow();

                const auto visibleEvents = computeVisibleEvents();

                auto event = visibleEvents[row];

                auto pitchEvent =
                    std::dynamic_pointer_cast<PitchBendEvent>(event);
                auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
                auto sysexEvent =
                    std::dynamic_pointer_cast<SystemExclusiveEvent>(event);
                auto maybeEmptyEvent =
                    std::dynamic_pointer_cast<EmptyEvent>(event);

                if (pitchEvent || mixerEvent || sysexEvent || maybeEmptyEvent)
                {
                    return;
                }

                auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);
                auto pgmChangeEvent =
                    std::dynamic_pointer_cast<ProgramChangeEvent>(event);
                auto chPressEvent =
                    std::dynamic_pointer_cast<ChannelPressureEvent>(event);
                auto polyPressEvent =
                    std::dynamic_pointer_cast<PolyPressureEvent>(event);
                auto controlChangeEvent =
                    std::dynamic_pointer_cast<ControlChangeEvent>(event);

                auto column = getActiveColumn();

                bool isA = column == "a";
                bool isB = column == "b";
                bool isC = column == "c";
                bool isD = column == "d";
                bool isE = column == "e";

                if ((polyPressEvent || controlChangeEvent) && isA)
                {
                    return;
                }

                auto editMultipleScreen =
                    mpc.screens->get<ScreenId::EditMultipleScreen>();

                auto track = sequencer->getActiveTrack();
                if (noteEvent && track->getBus() != 0)
                {
                    if (isA)
                    {
                        editMultipleScreen->setChangeNoteTo(
                            noteEvent->getNote());
                    }
                    else if (isB)
                    {
                        editMultipleScreen->setVariationType(
                            noteEvent->getVariationType());
                    }
                    else if (isC)
                    {
                        editMultipleScreen->setVariationType(
                            noteEvent->getVariationType());
                        editMultipleScreen->setVariationValue(
                            noteEvent->getVariationValue());
                    }
                    else if (isD)
                    {
                        editMultipleScreen->setEditValue(
                            *noteEvent->getDuration());
                    }
                    else if (isE)
                    {
                        editMultipleScreen->setEditValue(
                            noteEvent->getVelocity());
                    }
                }

                if (noteEvent && track->getBus() == 0)
                {
                    if (isA)
                    {
                        editMultipleScreen->setChangeNoteTo(
                            noteEvent->getNote());
                    }
                    else if (isB)
                    {
                        editMultipleScreen->setEditValue(
                            *noteEvent->getDuration());
                    }
                    else if (isC)
                    {
                        editMultipleScreen->setEditValue(
                            noteEvent->getVelocity());
                    }
                }
                else if (pgmChangeEvent)
                {
                    editMultipleScreen->setEditValue(0);
                }
                else if (chPressEvent)
                {
                    editMultipleScreen->setEditValue(chPressEvent->getAmount());
                }
                else if (polyPressEvent)
                {
                    editMultipleScreen->setEditValue(
                        polyPressEvent->getAmount());
                }
                else if (controlChangeEvent)
                {
                    editMultipleScreen->setEditValue(
                        controlChangeEvent->getAmount());
                }

                setSelectedEvent(visibleEvents[row]);
                setSelectedEvents();
                setSelectedParameterLetter(column);
                openScreenById(ScreenId::EditMultipleScreen);
            }

            break;
        }
        case 4:
            if (!placeHolder.empty())
            {
                openScreenById(ScreenId::PasteEventScreen);
            }
            break;
        case 5:
            if (selectionStartIndex == -1)
            {
                if (focusedFieldName.length() == 2)
                {
                    auto eventNumber = getActiveRow();
                    const auto visibleEvents = computeVisibleEvents();
                    auto event = visibleEvents[eventNumber];
                    auto noteEvent =
                        std::dynamic_pointer_cast<NoteOnEvent>(event);

                    if (noteEvent)
                    {
                        adhocPlayNoteEvent(noteEvent);
                    }
                }
            }
            else
            {
                clearSelection();
            }
            break;
    }
}

bool StepEditorScreen::paramIsLetter(const std::string &letter)
{
    return ls->getFocusedFieldName().find(letter) != std::string::npos;
}

void StepEditorScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    auto track = sequencer->getActiveTrack();

    if (focusedFieldName == "view")
    {
        setView(view + i);
    }
    else if (focusedFieldName == "now0")
    {
        setSequencerTickPos(
            [&]
            {
                sequencer->setBar(sequencer->getCurrentBarIndex() + i);
            });
    }
    else if (focusedFieldName == "now1")
    {
        setSequencerTickPos(
            [&]
            {
                sequencer->setBeat(sequencer->getCurrentBeatIndex() + i);
            });
    }
    else if (focusedFieldName == "now2")
    {
        setSequencerTickPos(
            [&]
            {
                sequencer->setClock(sequencer->getCurrentClockNumber() + i);
            });
    }
    else if (focusedFieldName == "tcvalue")
    {
        auto screen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
        auto noteValue = screen->getNoteValue();
        screen->setNoteValue(noteValue + i);
    }
    else if (focusedFieldName == "fromnote" && view == 1)
    {
        if (track->getBus() != 0)
        {
            setFromNote(fromNote + i);
        }
        if (track->getBus() == 0)
        {
            setNoteA(noteA + i);
        }
    }
    else if (focusedFieldName == "tonote")
    {
        setNoteB(noteB + i);
    }
    else if (focusedFieldName == "fromnote" && view == 3)
    {
        setControl(control + i);
    }
    else if (focusedFieldName.length() == 2)
    {
        const auto eventNumber = getActiveRow();
        const auto visibleEvents = computeVisibleEvents();

        if (auto sysEx = std::dynamic_pointer_cast<SystemExclusiveEvent>(
                visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                sysEx->setByteA(sysEx->getByteA() + i);
            }
            else if (paramIsLetter("b"))
            {
                sysEx->setByteB(sysEx->getByteB() + i);
            }
        }
        else if (auto channelPressure =
                     std::dynamic_pointer_cast<ChannelPressureEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                channelPressure->setAmount(channelPressure->getAmount() + i);
            }
        }
        else if (auto polyPressure =
                     std::dynamic_pointer_cast<PolyPressureEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                polyPressure->setNote(polyPressure->getNote() + i);
            }
            else if (paramIsLetter("b"))
            {
                polyPressure->setAmount(polyPressure->getAmount() + i);
            }
        }
        else if (auto controlChange =
                     std::dynamic_pointer_cast<ControlChangeEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                controlChange->setController(controlChange->getController() +
                                             i);
            }
            else if (paramIsLetter("b"))
            {
                controlChange->setAmount(controlChange->getAmount() + i);
            }
        }
        else if (auto programChange =
                     std::dynamic_pointer_cast<ProgramChangeEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                programChange->setProgram(programChange->getProgram() + i);
            }
        }
        else if (auto pitchBend = std::dynamic_pointer_cast<PitchBendEvent>(
                     visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                pitchBend->setAmount(pitchBend->getAmount() + i);
            }
        }
        else if (auto mixer = std::dynamic_pointer_cast<MixerEvent>(
                     visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                mixer->setParameter(mixer->getParameter() + i);
            }
            else if (paramIsLetter("b"))
            {
                mixer->setPadNumber(mixer->getPad() + i);
            }
            else if (paramIsLetter("c"))
            {
                mixer->setValue(mixer->getValue() + i);
            }
        }
        else if (auto note = std::dynamic_pointer_cast<NoteOnEvent>(
                     visibleEvents[eventNumber]);
                 track->getBus() == 0 && note)
        {
            if (paramIsLetter("a"))
            {
                note->setNote(note->getNote() + i);
            }
            else if (paramIsLetter("b"))
            {
                note->setDuration(note->getDuration() + i);
            }
            else if (paramIsLetter("c"))
            {
                note->setVelocity(note->getVelocity() + i);
            }
        }
        else if (note && track->getBus() != 0)
        {
            if (paramIsLetter("a"))
            {
                if (note->getNote() + i > 98)
                {
                    if (note->getNote() != 98)
                    {
                        note->setNote(98);
                    }

                    return;
                }
                else if (note->getNote() + i < 35)
                {
                    if (note->getNote() != 35)
                    {
                        note->setNote(35);
                    }

                    return;
                }
                else if (note->getNote() < 35)
                {
                    note->setNote(35);
                    return;
                }
                else if (note->getNote() > 98)
                {
                    note->setNote(98);
                    return;
                }

                note->setNote(note->getNote() + i);
            }
            else if (paramIsLetter("b"))
            {
                note->incrementVariationType(i);
            }
            else if (paramIsLetter("c"))
            {
                note->setVariationValue(note->getVariationValue() + i);
            }
            else if (paramIsLetter("d"))
            {
                note->setDuration(note->getDuration() + i);
            }
            else if (paramIsLetter("e"))
            {
                note->setVelocity(note->getVelocity() + i);
            }
        }
    }

    refreshSelection();
}

void StepEditorScreen::setSequencerTickPos(
    const std::function<void()> &tickPosSetter)
{
    storeColumnForEventAtActiveRow();

    auto oldTickPos = sequencer->getTickPosition();

    tickPosSetter();

    if (oldTickPos != sequencer->getTickPosition())
    {
        auto track = sequencer->getActiveTrack();
        track->removeDoubles();
        resetYPosAndYOffset();
        restoreColumnForEventAtActiveRow();
    }

    adhocPlayNoteEventsAtCurrentPosition();
}

void StepEditorScreen::prevStepEvent()
{
    setSequencerTickPos(
        [&]
        {
            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::GO_TO)
                    ->isPressed())
            {
                sequencer->goToPreviousEvent();
            }
            else
            {
                sequencer->goToPreviousStep();
            }
        });
}

void StepEditorScreen::nextStepEvent()
{
    setSequencerTickPos(
        [&]
        {
            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::GO_TO)
                    ->isPressed())
            {
                sequencer->goToNextEvent();
            }
            else
            {
                sequencer->goToNextStep();
            }
        });
}

void StepEditorScreen::prevBarStart()
{
    setSequencerTickPos(
        [&]
        {
            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::GO_TO)
                    ->isPressed())
            {
                sequencer->setBar(0);
            }
            else
            {
                sequencer->setBar(sequencer->getCurrentBarIndex() - 1);
            }
        });
}

void StepEditorScreen::nextBarEnd()
{
    setSequencerTickPos(
        [&]
        {
            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::GO_TO)
                    ->isPressed())
            {
                sequencer->setBar(
                    sequencer->getActiveSequence()->getLastBarIndex() + 1);
            }
            else
            {
                sequencer->setBar(sequencer->getCurrentBarIndex() + 1);
            }
        });
}

void StepEditorScreen::left()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2 && getActiveColumn() == "a")
    {
        lastRow = getActiveRow();
        ls->setFocus("view");
    }
    else
    {
        ScreenComponent::left();
    }

    checkSelection();
    refreshSelection();
}

void StepEditorScreen::right()
{
    ScreenComponent::right();
    checkSelection();
    refreshSelection();
}

void StepEditorScreen::up()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        const auto srcLetter = focusedFieldName.substr(0, 1);
        const int srcNumber = stoi(focusedFieldName.substr(1, 1));
        const auto visibleEvents = computeVisibleEvents();

        if (mpc.getHardware()
                ->getButton(hardware::ComponentId::SHIFT)
                ->isPressed() &&
            selectionStartIndex == -1 &&
            std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[srcNumber]))
        {
            return;
        }

        if (!mpc.getHardware()
                 ->getButton(hardware::ComponentId::SHIFT)
                 ->isPressed() &&
            srcNumber == 0 && yOffset == 0)
        {
            clearSelection();
            auto eventType = visibleEvents[srcNumber]->getTypeName();
            lastColumn[eventType] = srcLetter;
            lastRow = 0;
            ls->setFocus("view");
            refreshSelection();
            return;
        }

        if (srcNumber == 0 && yOffset != 0)
        {
            auto oldEventType = visibleEvents[srcNumber]->getTypeName();
            lastColumn[oldEventType] = srcLetter;

            setyOffset(yOffset - 1);

            auto newEventType = visibleEvents[srcNumber]->getTypeName();

            ls->setFocus(lastColumn[newEventType] + std::to_string(srcNumber));

            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::SHIFT)
                    ->isPressed())
            {
                setSelectionEndIndex(srcNumber + yOffset);
            }

            refreshSelection();
            return;
        }

        downOrUp(-1);
    }
}

void StepEditorScreen::down()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const auto eventsAtCurrentTick = computeEventsAtCurrentTick();
    const auto visibleEvents = computeVisibleEvents(eventsAtCurrentTick);

    if (focusedFieldName == "view" ||
        focusedFieldName.find("now") != std::string::npos ||
        focusedFieldName == "fromnote" || focusedFieldName == "tonote")
    {
        auto eventType = visibleEvents[lastRow]->getTypeName();
        ls->setFocus(lastColumn[eventType] + std::to_string(lastRow));
        return;
    }

    if (focusedFieldName.length() == 2)
    {
        const auto srcLetter = focusedFieldName.substr(0, 1);
        const int srcNumber = stoi(focusedFieldName.substr(1, 1));

        if (srcNumber == 3)
        {
            if (yOffset + 4 == eventsAtCurrentTick.size())
            {
                return;
            }

            auto oldEventType = visibleEvents[srcNumber]->getTypeName();
            lastColumn[oldEventType] = srcLetter;

            setyOffset(yOffset + 1);

            auto newEventType = visibleEvents[srcNumber]->getTypeName();
            auto newColumn = lastColumn[newEventType];

            ls->setFocus(newColumn + "3");

            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::SHIFT)
                    ->isPressed() &&
                std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[3]))
            {
                setSelectionEndIndex(srcNumber + yOffset);
            }

            refreshSelection();
            return;
        }

        downOrUp(1);
    }
}

void StepEditorScreen::shift()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        auto eventNumber = getActiveRow();
        setSelectionStartIndex(eventNumber + yOffset);
    }
}

void StepEditorScreen::downOrUp(int increment)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() == 2)
    {
        const auto srcLetter = focusedFieldName.substr(0, 1);
        int srcNumber = stoi(focusedFieldName.substr(1, 1));

        if (srcNumber + increment != -1)
        {
            const auto visibleEvents = computeVisibleEvents();

            if (visibleEvents[srcNumber + increment])
            {
                auto oldEventType = visibleEvents[srcNumber]->getTypeName();
                lastColumn[oldEventType] = srcLetter;

                auto newEventType =
                    visibleEvents[srcNumber + increment]->getTypeName();
                ls->setFocus(lastColumn[newEventType] +
                             std::to_string(srcNumber + increment));
            }
        }

        if (mpc.getHardware()
                ->getButton(hardware::ComponentId::SHIFT)
                ->isPressed())
        {
            setSelectionEndIndex(srcNumber + increment + yOffset);
        }
        else
        {
            checkSelection();
            refreshSelection();
        }
    }
}

void StepEditorScreen::refreshSelection()
{
    auto firstEventIndex = std::min(selectionStartIndex, selectionEndIndex);
    auto lastEventIndex = std::max(selectionStartIndex, selectionEndIndex);

    bool somethingSelected = false;

    if (firstEventIndex != -1)
    {
        for (int i = 0; i < EVENT_ROW_COUNT; i++)
        {
            int absoluteEventNumber = i + yOffset;
            auto eventRow =
                findChild<EventRow>("event-row-" + std::to_string(i));

            if (absoluteEventNumber >= firstEventIndex &&
                absoluteEventNumber < lastEventIndex + 1)
            {
                eventRow->setSelected(true);
                somethingSelected = true;
            }
            else
            {
                eventRow->setSelected(false);
            }
        }
    }
    else
    {
        for (int i = 0; i < EVENT_ROW_COUNT; i++)
        {
            auto eventRow =
                findChild<EventRow>("event-row-" + std::to_string(i));
            eventRow->setSelected(false);
        }
    }

    if (somethingSelected)
    {
        ls->setFunctionKeysArrangement(1);
    }
}

std::vector<std::shared_ptr<Event>> StepEditorScreen::computeVisibleEvents(
    const std::vector<std::shared_ptr<Event>> &eventsAtCurrentTick)
{
    std::vector<std::shared_ptr<Event>> result(4);
    int firstVisibleEventIndex = yOffset;
    int visibleEventCounter = 0;

    std::optional<std::vector<std::shared_ptr<Event>>> ownedEvents;

    const std::vector<std::shared_ptr<Event>> &eventsAtCurrentTickToUse =
        eventsAtCurrentTick.empty()
            ? (ownedEvents = computeEventsAtCurrentTick(), *ownedEvents)
            : eventsAtCurrentTick;

    for (int i = 0; i < EVENT_ROW_COUNT; i++)
    {
        result[visibleEventCounter] =
            eventsAtCurrentTickToUse[i + firstVisibleEventIndex];
        visibleEventCounter++;

        if (visibleEventCounter > 3 ||
            visibleEventCounter > eventsAtCurrentTickToUse.size() - 1)
        {
            break;
        }
    }

    return result;
}

std::vector<std::shared_ptr<Event>>
StepEditorScreen::computeEventsAtCurrentTick()
{
    std::vector<std::shared_ptr<Event>> result;

    auto track = sequencer->getActiveTrack();

    for (auto &event : track->getEvents())
    {
        if (event->getTick() == sequencer->getTickPosition())
        {
            if ((view == 0 || view == 1) &&
                std::dynamic_pointer_cast<NoteOnEvent>(event))
            {
                auto ne = std::dynamic_pointer_cast<NoteOnEvent>(event);

                if (track->getBus() != 0)
                {
                    if (fromNote == 34 || view == 0)
                    {
                        result.push_back(ne);
                    }
                    else if (fromNote != 34 && fromNote == ne->getNote())
                    {
                        result.push_back(ne);
                    }
                }
                else
                {
                    if ((ne->getNote() >= noteA && ne->getNote() <= noteB) ||
                        view == 0)
                    {
                        result.push_back(ne);
                    }
                }
            }

            if ((view == 0 || view == 2) &&
                std::dynamic_pointer_cast<PitchBendEvent>(event))
            {
                result.push_back(event);
            }

            if ((view == 0 || view == 3) &&
                std::dynamic_pointer_cast<ControlChangeEvent>(event))
            {
                if (control == -1)
                {
                    result.push_back(event);
                }
                if (control ==
                    std::dynamic_pointer_cast<ControlChangeEvent>(event)
                        ->getController())
                {
                    result.push_back(event);
                }
            }
            if ((view == 0 || view == 4) &&
                std::dynamic_pointer_cast<ProgramChangeEvent>(event))
            {
                result.push_back(event);
            }

            if ((view == 0 || view == 5) &&
                std::dynamic_pointer_cast<ChannelPressureEvent>(event))
            {
                result.push_back(event);
            }

            if ((view == 0 || view == 6) &&
                std::dynamic_pointer_cast<PolyPressureEvent>(event))
            {
                result.push_back(event);
            }

            if ((view == 0 || view == 7) &&
                (std::dynamic_pointer_cast<SystemExclusiveEvent>(event) ||
                 std::dynamic_pointer_cast<MixerEvent>(event)))
            {
                result.push_back(event);
            }
        }
    }

    result.push_back(emptyEvent);
    return result;
}

void StepEditorScreen::refreshEventRows()
{
    const auto visibleEvents = computeVisibleEvents();

    for (int i = 0; i < EVENT_ROW_COUNT; i++)
    {
        auto eventRow = findChild<EventRow>("event-row-" + std::to_string(i));
        auto event = visibleEvents[i];

        if (event)
        {
            eventRow->Hide(false);
            eventRow->setBus(sequencer->getActiveTrack()->getBus());
        }
        else
        {
            eventRow->Hide(true);
        }

        eventRow->setEvent(event);
        eventRow->init();
    }
}

void StepEditorScreen::updateComponents()
{
    auto track = sequencer->getActiveTrack();

    if (view == 1 && track->getBus() != 0)
    {
        findField("fromnote")->Hide(false);
        findField("fromnote")->setSize(37, 9);
        findField("fromnote")->setLocation(67, 0);
        findLabel("tonote")->Hide(true);
        findField("tonote")->Hide(true);
    }
    else if (view == 1 && track->getBus() == 0)
    {
        findField("fromnote")->Hide(false);
        findField("fromnote")->setLocation(61, 0);
        findField("fromnote")->setSize(47, 9);
        findField("tonote")->setSize(47, 9);
        findLabel("tonote")->Hide(false);
        findLabel("tonote")->setText("-");
        findField("tonote")->Hide(false);
    }
    else if (view == 3)
    {
        auto fromNoteField = findField("fromnote");
        fromNoteField->Hide(false);
        fromNoteField->setLocation(60, 0);
        fromNoteField->setSize(104, 9);

        findLabel("tonote")->Hide(true);
        findField("tonote")->Hide(true);
    }
    else if (view != 1 && view != 3)
    {
        findField("fromnote")->Hide(true);
        findLabel("tonote")->Hide(true);
        findField("tonote")->Hide(true);
    }
}

void StepEditorScreen::setViewNotesText()
{
    auto track = sequencer->getActiveTrack();

    if (view == 1 && track->getBus() != 0)
    {
        if (fromNote == 34)
        {
            findField("fromnote")->setText("ALL");
        }
        else
        {
            auto program = getProgramOrThrow();
            auto padName =
                sampler->getPadName(program->getPadIndexFromNote(fromNote));
            findField("fromnote")
                ->setText(std::to_string(fromNote) + "/" + padName);
        }
    }
    else if (view == 1 && track->getBus() == 0)
    {
        findField("fromnote")
            ->setText(StrUtil::padLeft(std::to_string(noteA), " ", 3) + "(" +
                      mpc::Util::noteNames()[noteA] + u8"\u00D4");
        findField("tonote")->setText(
            StrUtil::padLeft(std::to_string(noteB), " ", 3) + "(" +
            mpc::Util::noteNames()[noteB] + u8"\u00D4");
    }
    else if (view == 3)
    {
        if (control == -1)
        {
            findField("fromnote")->setText("   -    ALL");
        }
        else
        {
            findField("fromnote")
                ->setText(StrUtil::padLeft(std::to_string(control), " ", 3) +
                          "-" + EventRow::controlNames[control]);
        }
    }

    findField("view")->setText(viewNames[view]);
    auto newWidth = findField("view")->getText().length() * 6 + 1;
    findField("view")->setSize(newWidth, 9);
}

void StepEditorScreen::setView(int i)
{
    view = std::clamp(i, 0, 7);

    displayView();
    updateComponents();
    setViewNotesText();
    setyOffset(0);
    findChild<Rectangle>()->SetDirty();
}

void StepEditorScreen::setNoteA(int i)
{
    noteA = std::clamp(i, 0, 127);

    if (noteA > noteB)
    {
        noteB = noteA;
    }

    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setNoteB(int i)
{
    noteB = std::clamp(i, 0, 127);

    if (noteB < noteA)
    {
        noteA = noteB;
    }

    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setControl(int i)
{
    control = std::clamp(i, -1, 127);

    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setyOffset(int i)
{
    if (i < 0)
    {
        i = 0;
    }

    yOffset = i;

    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setFromNote(int i)
{
    fromNote = std::clamp(i, 34, 98);

    setViewNotesText();
    displayView();
    updateComponents();
    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setSelectionStartIndex(int i)
{
    const auto eventsAtCurrentTick = computeEventsAtCurrentTick();

    if (std::dynamic_pointer_cast<EmptyEvent>(eventsAtCurrentTick[i]))
    {
        return;
    }

    selectionStartIndex = i;
    selectionEndIndex = i;

    ls->setFunctionKeysArrangement(1);
    refreshSelection();
}

void StepEditorScreen::clearSelection()
{
    selectionStartIndex = -1;
    selectionEndIndex = -1;
    ls->setFunctionKeysArrangement(0);
    refreshSelection();
}

void StepEditorScreen::setSelectionEndIndex(int i)
{
    if (i == -1)
    {
        i = -1;
    }

    selectionEndIndex = i;
    refreshSelection();
}

void StepEditorScreen::setSelectedEvents()
{
    selectedEvents.clear();
    auto firstEventIndex = selectionStartIndex;
    auto lastEventIndex = selectionEndIndex;

    if (firstEventIndex > lastEventIndex)
    {
        firstEventIndex = selectionEndIndex;
        lastEventIndex = selectionStartIndex;
    }

    const auto eventsAtCurrentTick = computeEventsAtCurrentTick();

    for (int i = firstEventIndex; i < lastEventIndex + 1; i++)
    {
        if (i >= eventsAtCurrentTick.size())
        {
            break;
        }

        selectedEvents.push_back(eventsAtCurrentTick[i]);
    }
}

void StepEditorScreen::checkSelection()
{
    std::string focus = ls->getFocusedFieldName();

    if (focus.length() != 2)
    {
        clearSelection();
        return;
    }

    int row = stoi(focus.substr(1, 1));
    int eventIndex = row + yOffset;

    if (selectionStartIndex > selectionEndIndex)
    {
        std::swap(selectionStartIndex, selectionEndIndex);
    }

    if (eventIndex < selectionStartIndex || eventIndex > selectionEndIndex)
    {
        clearSelection();
    }
}

void StepEditorScreen::setSelectedEvent(std::weak_ptr<Event> event)
{
    selectedEvent = event.lock();
}

void StepEditorScreen::setSelectedParameterLetter(std::string str)
{
    selectedParameterLetter = str;
}

void StepEditorScreen::removeEvents()
{
    auto firstEventIndex = selectionStartIndex;
    auto lastEventIndex = selectionEndIndex;

    if (firstEventIndex > lastEventIndex)
    {
        firstEventIndex = selectionEndIndex;
        lastEventIndex = selectionStartIndex;
    }

    const auto eventsAtCurrentTick = computeEventsAtCurrentTick();

    for (int i = 0; i < eventsAtCurrentTick.size(); i++)
    {
        if (i >= firstEventIndex && i <= lastEventIndex)
        {
            auto event = eventsAtCurrentTick[i];
            if (!std::dynamic_pointer_cast<EmptyEvent>(event))
            {
                auto track = sequencer->getActiveTrack();
                track->removeEvent(event);
            }
        }
    }

    clearSelection();
    setyOffset(0);
}

void StepEditorScreen::displayView()
{
    findField("view")->setText(viewNames[view]);
}

std::vector<std::shared_ptr<Event>> &StepEditorScreen::getSelectedEvents()
{
    return selectedEvents;
}

std::string StepEditorScreen::getSelectedParameterLetter()
{
    return selectedParameterLetter;
}

std::shared_ptr<Event> StepEditorScreen::getSelectedEvent()
{
    return selectedEvent;
}

std::vector<std::shared_ptr<Event>> &StepEditorScreen::getPlaceHolder()
{
    return placeHolder;
}

int StepEditorScreen::getYOffset()
{
    return yOffset;
}

void StepEditorScreen::adhocPlayNoteEvent(
    const std::shared_ptr<mpc::sequencer::NoteOnEvent> &noteEvent)
{
    const auto adhoc = std::make_shared<NoteOnEventPlayOnly>(*noteEvent);
    auto track = sequencer->getActiveTrack();
    mpc.getEventHandler()->handleFinalizedEvent(adhoc, track.get());
}

void StepEditorScreen::resetYPosAndYOffset()
{
    yOffset = 0;

    if (getActiveRow() <= 0)
    {
        return;
    }

    ls->setFocus("a0");
}

std::string StepEditorScreen::getActiveColumn()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.size() != 2)
    {
        return "";
    }

    return focusedFieldName.substr(0, 1);
}

int StepEditorScreen::getActiveRow()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.length() != 2)
    {
        return -1;
    }

    return stoi(focusedFieldName.substr(1, 1));
}

void StepEditorScreen::storeColumnForEventAtActiveRow()
{
    const int row = getActiveRow();
    const std::string column = getActiveColumn();

    if (row == -1 || column.empty())
    {
        return;
    }

    lastColumn[computeVisibleEvents()[row]->getTypeName()] = column;
}

void StepEditorScreen::restoreColumnForEventAtActiveRow()
{
    const int row = getActiveRow();

    if (row == -1)
    {
        return;
    }

    const auto desiredColumn =
        lastColumn[computeVisibleEvents()[row]->getTypeName()];

    ls->setFocus(desiredColumn + std::to_string(row));
}

void StepEditorScreen::adhocPlayNoteEventsAtCurrentPosition()
{
    auto tick = sequencer->getTickPosition();
    auto track = sequencer->getActiveTrack();
    for (auto &e : track->getEventRange(tick, tick))
    {
        auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(e);
        if (noteEvent)
        {
            adhocPlayNoteEvent(noteEvent);
        }
    }
}
