#include "StepEditorScreen.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/InsertEventScreen.hpp"

#include "audiomidi/EventHandler.hpp"

#include "hardware/Hardware.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/EditMultipleScreen.hpp"

#include "lcdgui/Rectangle.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/EventEquals.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/EventRow.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

constexpr int EVENT_ROW_COUNT = 4;

StepEditorScreen::StepEditorScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "step-editor", layerIndex),
      WithLocateStepEventBarSequence(
          mpc.getHardware()->getButton(hardware::ComponentId::GO_TO).get(),
          mpc.getSequencer().get())
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

    addReactiveBinding({[&]
                        {
                            const auto original = computeVisibleEvents();
                            std::vector<std::shared_ptr<EventRef>> clones;
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
                            return eventsEqual(a, b);
                        }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getTransport()->getTickPosition();
         },
         [&](auto)
         {
             if (isFirstTickPosChangeAfterScreenHasBeenOpened)
             {
                 isFirstTickPosChangeAfterScreenHasBeenOpened = false;
             }
             else
             {
                 const auto track = sequencer.lock()->getSelectedTrack();
                 track->removeDoubles();
                 resetYPosAndYOffset();
                 restoreColumnForEventAtActiveRow();
                 adhocPlayNoteEventsAtCurrentPosition();
             }

             findField("now0")->setTextPadded(
                 sequencer.lock()->getTransport()->getCurrentBarIndex() + 1,
                 "0");
             findField("now1")->setTextPadded(
                 sequencer.lock()->getTransport()->getCurrentBeatIndex() + 1,
                 "0");
             findField("now2")->setTextPadded(
                 sequencer.lock()->getTransport()->getCurrentClockNumber(),
                 "0");
         }});
}

void StepEditorScreen::openWindow()
{
    openScreenById(ScreenId::StepEditOptionsScreen);
}

void StepEditorScreen::open()
{
    sequencer.lock()->copySelectedSequenceToUndoSequence();

    findField("tonote")->setLocation(115, 0);
    findLabel("fromnote")->Hide(true);

    lastRow = 0;

    if (const auto track = sequencer.lock()->getSelectedTrack();
        isDrumBusType(track->getBusType()))
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

    findField("now0")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBarIndex() + 1, "0");
    findField("now1")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBeatIndex() + 1, "0");
    findField("now2")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentClockNumber(), "0");

    const auto eventsAtCurrentTick = computeEventsAtCurrentTick();

    if (ls.lock()->isPreviousScreen({ScreenId::InsertEventScreen}))
    {
        const auto insertEventScreen =
            mpc.screens->get<ScreenId::InsertEventScreen>();

        if (insertEventScreen->isEventAddedBeforeLeavingTheScreen())
        {
            const auto eventCount =
                static_cast<int>(eventsAtCurrentTick.size()) - 1;
            const auto event = eventsAtCurrentTick[eventCount - 1];
            const auto eventType = event->getTypeName();

            if (eventCount > 4)
            {
                ls.lock()->setFocus(lastColumn[eventType] + "3");
                setyOffset(eventCount - 4);
            }
            else
            {
                const auto row = eventCount - 1;
                ls.lock()->setFocus(lastColumn[eventType] +
                                    std::to_string(row));
            }
        }
    }

    const auto visibleEvents = computeVisibleEvents(eventsAtCurrentTick);

    if (ls.lock()->isPreviousScreenNot(
            {ScreenId::StepTcScreen, ScreenId::InsertEventScreen,
             ScreenId::PasteEventScreen, ScreenId::EditMultipleScreen}))
    {
        const auto eventType = visibleEvents[0]->getTypeName();
        ls.lock()->setFocus(lastColumn[eventType] + "0");
    }

    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::close()
{
    const auto lockedSequencer = sequencer.lock();

    lockedSequencer->getTransport()->stopMetronomeOnly();

    const auto track = lockedSequencer->getSelectedTrack();

    for (const auto &e : computeEventsAtCurrentTick())
    {
        if (e->snapshot.type == EventType::NoteOn && e->snapshot.beingRecorded)
        {
            track->finalizeNoteEventNonLive(e->handle, Duration(1));
        }
    }

    mpc.getEngineHost()->flushNoteOffs();
    isFirstTickPosChangeAfterScreenHasBeenOpened = true;

    storeColumnForEventAtActiveRow();

    if (!ls.lock()->isCurrentScreenOrChildOf(ScreenId::StepEditorScreen))
    {
        track->removeDoubles();
        sequencer.lock()->resetUndo();
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
                ls.lock()->setFocus("a0");
                return;
            }

            if (!std::dynamic_pointer_cast<EmptyEvent>(visibleEvents[rowIndex]))
            {
                auto track = sequencer.lock()->getSelectedTrack();

                track->removeEvent(visibleEvents[rowIndex]);

                if (rowIndex == 2 && yOffset > 0)
                {
                    yOffset--;
                }
            }

            refreshEventRows();
            refreshSelection();

            const std::string newEventType =
                visibleEvents[rowIndex]->getTypeName();

            ls.lock()->setFocus(lastColumn[newEventType] +
                                std::to_string(rowIndex));
            break;
        }
        case 3:
        {
            bool posIsLastTick =
                sequencer.lock()->getTransport()->getTickPosition() ==
                sequencer.lock()->getSelectedSequence()->getLastTick();

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

                auto track = sequencer.lock()->getSelectedTrack();
                if (noteEvent && isDrumBusType(track->getBusType()))
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
                            noteEvent->getDuration());
                    }
                    else if (isE)
                    {
                        editMultipleScreen->setEditValue(
                            noteEvent->getVelocity());
                    }
                }

                if (noteEvent && isMidiBusType(track->getBusType()))
                {
                    if (isA)
                    {
                        editMultipleScreen->setChangeNoteTo(
                            noteEvent->getNote());
                    }
                    else if (isB)
                    {
                        editMultipleScreen->setEditValue(
                            noteEvent->getDuration());
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

                    if (auto noteEvent =
                            std::dynamic_pointer_cast<NoteOnEvent>(event))
                    {
                        adhocPlayNoteEvent(*noteEvent->handle);
                    }
                }
            }
            else
            {
                clearSelection();
            }
            break;
        default:;
    }
}

bool StepEditorScreen::paramIsLetter(const std::string &letter) const
{
    const auto focusedFieldName = ls.lock()->getFocusedFieldName();
    return focusedFieldName.find(letter) != std::string::npos;
}

void StepEditorScreen::turnWheel(const int increment)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const auto track = sequencer.lock()->getSelectedTrack();
    const auto transport = sequencer.lock()->getTransport();
    const auto tickPosition = transport->getTickPosition();

    if (focusedFieldName == "view")
    {
        setView(view + increment);
    }
    else if (focusedFieldName == "now0")
    {
        if (tickPosition == 0 && increment < 0)
        {
            adhocPlayNoteEventsAtCurrentPosition();
            return;
        }

        setSequencerTickPos(
            [&]
            {
                transport->setBar(transport->getCurrentBarIndex() + increment);
            });
    }
    else if (focusedFieldName == "now1")
    {
        if (transport->getCurrentBeatIndex() == 0 && increment < 0)
        {
            adhocPlayNoteEventsAtCurrentPosition();
            return;
        }

        if (increment > 0 &&
            transport->getCurrentBeatIndex() ==
                sequencer.lock()
                        ->getSelectedSequence()
                        ->getTimeSignatureFromTickPos(tickPosition)
                        .numerator -
                    1)
        {
            adhocPlayNoteEventsAtCurrentPosition();
            return;
        }

        setSequencerTickPos(
            [&]
            {
                transport->setBeat(transport->getCurrentBeatIndex() +
                                   increment);
            });
    }
    else if (focusedFieldName == "now2")
    {
        if (transport->getCurrentClockNumber() == 0 && increment < 0)
        {
            adhocPlayNoteEventsAtCurrentPosition();
            return;
        }

        const auto timeSig = sequencer.lock()
                                 ->getSelectedSequence()
                                 ->getTimeSignatureFromTickPos(tickPosition);
        const auto ticksPerBeat = 96 * (4 / timeSig.denominator);

        if (increment > 0 &&
            transport->getCurrentClockNumber() == ticksPerBeat - 1)
        {
            adhocPlayNoteEventsAtCurrentPosition();
            return;
        }

        setSequencerTickPos(
            [&]
            {
                transport->setClock(transport->getCurrentClockNumber() +
                                    increment);
            });
    }
    else if (focusedFieldName == "tcvalue")
    {
        const auto screen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
        const auto noteValue = screen->getNoteValue();
        screen->setNoteValue(noteValue + increment);
    }
    else if (focusedFieldName == "fromnote" && view == 1)
    {
        if (isDrumBusType(track->getBusType()))
        {
            setFromNote(fromNote + increment);
        }
        else
        {
            assert(isMidiBusType(track->getBusType()));
            setNoteA(noteA + increment);
        }
    }
    else if (focusedFieldName == "tonote")
    {
        setNoteB(noteB + increment);
    }
    else if (focusedFieldName == "fromnote" && view == 3)
    {
        setControl(control + increment);
    }
    else if (focusedFieldName.length() == 2)
    {
        const auto eventNumber = getActiveRow();
        const auto visibleEvents = computeVisibleEvents();

        if (const auto sysEx = std::dynamic_pointer_cast<SystemExclusiveEvent>(
                visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                sysEx->setByteA(sysEx->getByteA() + increment);
            }
            else if (paramIsLetter("b"))
            {
                sysEx->setByteB(sysEx->getByteB() + increment);
            }
        }
        else if (const auto channelPressure =
                     std::dynamic_pointer_cast<ChannelPressureEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                channelPressure->setAmount(channelPressure->getAmount() +
                                           increment);
            }
        }
        else if (const auto polyPressure =
                     std::dynamic_pointer_cast<PolyPressureEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                polyPressure->setNote(polyPressure->getNote() + increment);
            }
            else if (paramIsLetter("b"))
            {
                polyPressure->setAmount(polyPressure->getAmount() + increment);
            }
        }
        else if (const auto controlChange =
                     std::dynamic_pointer_cast<ControlChangeEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                controlChange->setController(controlChange->getController() +
                                             increment);
            }
            else if (paramIsLetter("b"))
            {
                controlChange->setAmount(controlChange->getAmount() +
                                         increment);
            }
        }
        else if (const auto programChange =
                     std::dynamic_pointer_cast<ProgramChangeEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                programChange->setProgram(programChange->getProgram() +
                                          increment);
            }
        }
        else if (const auto pitchBend =
                     std::dynamic_pointer_cast<PitchBendEvent>(
                         visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                pitchBend->setAmount(pitchBend->getAmount() + increment);
            }
        }
        else if (const auto mixer = std::dynamic_pointer_cast<MixerEvent>(
                     visibleEvents[eventNumber]))
        {
            if (paramIsLetter("a"))
            {
                mixer->setParameter(mixer->getParameter() + increment);
            }
            else if (paramIsLetter("b"))
            {
                mixer->setPadNumber(mixer->getPad() + increment);
            }
            else if (paramIsLetter("c"))
            {
                mixer->setValue(mixer->getValue() + increment);
            }
        }
        else if (const auto note = std::dynamic_pointer_cast<NoteOnEvent>(
                     visibleEvents[eventNumber]);
                 isMidiBusType(track->getBusType()) && note)
        {
            if (paramIsLetter("a"))
            {
                note->setNote(note->getNote() + increment);
            }
            else if (paramIsLetter("b"))
            {
                note->setDuration(note->getDuration() + increment);
            }
            else if (paramIsLetter("c"))
            {
                note->setVelocity(note->getVelocity() + increment);
            }
        }
        else if (note && isDrumBusType(track->getBusType()))
        {
            if (paramIsLetter("a"))
            {
                if (note->getNote() + increment > MaxDrumNoteNumber)
                {
                    if (note->getNote() != MaxDrumNoteNumber)
                    {
                        note->setNote(MaxDrumNoteNumber);
                    }

                    return;
                }
                if (note->getNote() + increment < MinDrumNoteNumber)
                {
                    if (note->getNote() != MinDrumNoteNumber)
                    {
                        note->setNote(MinDrumNoteNumber);
                    }

                    return;
                }
                if (note->getNote() < MinDrumNoteNumber)
                {
                    note->setNote(MinDrumNoteNumber);
                    return;
                }
                if (note->getNote() > MaxDrumNoteNumber)
                {
                    note->setNote(MaxDrumNoteNumber);
                    return;
                }

                note->setNote(note->getNote() + increment);
            }
            else if (paramIsLetter("b"))
            {
                note->setVariationType(note->getVariationType() + increment);
            }
            else if (paramIsLetter("c"))
            {
                note->setVariationValue(note->getVariationValue() + increment);
            }
            else if (paramIsLetter("d"))
            {
                note->setDuration(note->getDuration() + increment);
            }
            else if (paramIsLetter("e"))
            {
                note->setVelocity(note->getVelocity() + increment);
            }
        }
    }

    refreshSelection();
}

void StepEditorScreen::setSequencerTickPos(
    const std::function<void()> &tickPosSetter)
{
    isFirstTickPosChangeAfterScreenHasBeenOpened = false;
    storeColumnForEventAtActiveRow();
    tickPosSetter();
}

void StepEditorScreen::prevStepEvent()
{
    mpc.getEngineHost()->flushNoteOffs();

    if (sequencer.lock()->getTransport()->getTickPosition() == 0)
    {
        adhocPlayNoteEventsAtCurrentPosition();
        return;
    }

    setSequencerTickPos(
        [&]
        {
            WithLocateStepEventBarSequence::prevStepEvent();
        });
}

void StepEditorScreen::nextStepEvent()
{
    mpc.getEngineHost()->flushNoteOffs();
    setSequencerTickPos(
        [&]
        {
            WithLocateStepEventBarSequence::nextStepEvent();
        });
}

void StepEditorScreen::prevBarStart()
{
    mpc.getEngineHost()->flushNoteOffs();

    if (sequencer.lock()->getTransport()->getTickPosition() == 0)
    {
        adhocPlayNoteEventsAtCurrentPosition();
        return;
    }

    setSequencerTickPos(
        [&]
        {
            WithLocateStepEventBarSequence::prevBarStart();
        });
}

void StepEditorScreen::nextBarEnd()
{
    mpc.getEngineHost()->flushNoteOffs();
    setSequencerTickPos(
        [&]
        {
            WithLocateStepEventBarSequence::nextBarEnd();
        });
}

void StepEditorScreen::left()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2 && getActiveColumn() == "a")
    {
        lastRow = getActiveRow();
        ls.lock()->setFocus("view");
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
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2)
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
            const auto eventType = visibleEvents[srcNumber]->getTypeName();
            lastColumn[eventType] = srcLetter;
            lastRow = 0;
            ls.lock()->setFocus("view");
            refreshSelection();
            return;
        }

        if (srcNumber == 0 && yOffset != 0)
        {
            const auto oldEventType = visibleEvents[srcNumber]->getTypeName();
            lastColumn[oldEventType] = srcLetter;

            setyOffset(yOffset - 1);

            const auto newEventType = visibleEvents[srcNumber]->getTypeName();

            ls.lock()->setFocus(lastColumn[newEventType] +
                                std::to_string(srcNumber));

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
        ls.lock()->setFocus(lastColumn[eventType] + std::to_string(lastRow));
        return;
    }

    if (focusedFieldName.length() == 2)
    {
        const auto srcLetter = focusedFieldName.substr(0, 1);

        if (const int srcNumber = stoi(focusedFieldName.substr(1, 1));
            srcNumber == 3)
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

            ls.lock()->setFocus(newColumn + "3");

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
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2)
    {
        const auto eventNumber = getActiveRow();
        setSelectionStartIndex(eventNumber + yOffset);
    }
}

void StepEditorScreen::downOrUp(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.length() == 2)
    {
        const auto srcLetter = focusedFieldName.substr(0, 1);
        const int srcNumber = stoi(focusedFieldName.substr(1, 1));

        if (srcNumber + increment != -1)
        {
            if (const auto visibleEvents = computeVisibleEvents();
                visibleEvents[srcNumber + increment])
            {
                const auto oldEventType =
                    visibleEvents[srcNumber]->getTypeName();
                lastColumn[oldEventType] = srcLetter;

                const auto newEventType =
                    visibleEvents[srcNumber + increment]->getTypeName();
                ls.lock()->setFocus(lastColumn[newEventType] +
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
    const auto firstEventIndex =
        std::min(selectionStartIndex, selectionEndIndex);
    const auto lastEventIndex =
        std::max(selectionStartIndex, selectionEndIndex);

    bool somethingSelected = false;

    if (firstEventIndex != -1)
    {
        for (int i = 0; i < EVENT_ROW_COUNT; i++)
        {
            const int absoluteEventNumber = i + yOffset;
            const auto eventRow =
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
            const auto eventRow =
                findChild<EventRow>("event-row-" + std::to_string(i));
            eventRow->setSelected(false);
        }
    }

    if (somethingSelected)
    {
        ls.lock()->setFunctionKeysArrangement(1);
    }
}

std::vector<std::shared_ptr<EventRef>> StepEditorScreen::computeVisibleEvents(
    const std::vector<std::shared_ptr<EventRef>> &eventsAtCurrentTick) const
{
    std::vector<std::shared_ptr<EventRef>> result(4);
    const int firstVisibleEventIndex = yOffset;
    int visibleEventCounter = 0;

    std::optional<std::vector<std::shared_ptr<EventRef>>> ownedEvents;

    const std::vector<std::shared_ptr<EventRef>> &eventsAtCurrentTickToUse =
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

std::vector<std::shared_ptr<EventRef>>
StepEditorScreen::computeEventsAtCurrentTick() const
{
    std::vector<std::shared_ptr<EventRef>> result;

    const auto track = sequencer.lock()->getSelectedTrack();
    const auto trackEvents = track->getEvents();
    const auto positionTicks =
        sequencer.lock()->getTransport()->getTickPosition();

    for (auto &event : track->getEvents())
    {
        if (event->getTick() == positionTicks)
        {
            if ((view == 0 || view == 1) &&
                std::dynamic_pointer_cast<NoteOnEvent>(event))
            {
                auto ne = std::dynamic_pointer_cast<NoteOnEvent>(event);

                if (isMidiBusType(track->getBusType()))
                {
                    if (fromNote == NoDrumNoteAssigned || view == 0)
                    {
                        result.push_back(ne);
                    }
                    else if (/*fromNote != NoDrumNoteAssigned &&*/ fromNote ==
                             ne->getNote())
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
        const auto eventRow =
            findChild<EventRow>("event-row-" + std::to_string(i));
        auto event = visibleEvents[i];

        if (event)
        {
            eventRow->Hide(false);
            eventRow->setBus(
                sequencer.lock()->getSelectedTrack()->getBusType());
        }
        else
        {
            eventRow->Hide(true);
        }

        eventRow->setEvent(event);
        eventRow->init();
    }
}

void StepEditorScreen::updateComponents() const
{
    if (const auto track = sequencer.lock()->getSelectedTrack();
        view == 1 && isMidiBusType(track->getBusType()))
    {
        findField("fromnote")->Hide(false);
        findField("fromnote")->setSize(37, 9);
        findField("fromnote")->setLocation(67, 0);
        findLabel("tonote")->Hide(true);
        findField("tonote")->Hide(true);
    }
    else if (view == 1 && isDrumBusType(track->getBusType()))
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
        const auto fromNoteField = findField("fromnote");
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

void StepEditorScreen::setViewNotesText() const
{
    if (const auto track = sequencer.lock()->getSelectedTrack();
        view == 1 && isMidiBusType(track->getBusType()))
    {
        if (fromNote == NoDrumNoteAssigned)
        {
            findField("fromnote")->setText("ALL");
        }
        else
        {
            const auto program = getProgramOrThrow();
            const auto padName = sampler.lock()->getPadName(
                program->getPadIndexFromNote(fromNote));
            findField("fromnote")
                ->setText(std::to_string(fromNote) + "/" + padName);
        }
    }
    else if (view == 1 && isDrumBusType(track->getBusType()))
    {
        findField("fromnote")
            ->setText(StrUtil::padLeft(std::to_string(noteA), " ", 3) + "(" +
                      Util::noteNames()[noteA] + u8"\u00D4");
        findField("tonote")->setText(
            StrUtil::padLeft(std::to_string(noteB), " ", 3) + "(" +
            Util::noteNames()[noteB] + u8"\u00D4");
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
    const auto newWidth = findField("view")->getText().length() * 6 + 1;
    findField("view")->setSize(newWidth, 9);
}

void StepEditorScreen::setView(const int i)
{
    view = std::clamp(i, 0, 7);

    displayView();
    updateComponents();
    setViewNotesText();
    setyOffset(0);
    findChild<Rectangle>()->SetDirty();
}

void StepEditorScreen::setNoteA(const NoteNumber noteNumber)
{
    noteA = std::clamp(noteNumber, MinNoteNumber, MaxNoteNumber);

    if (noteA > noteB)
    {
        noteB = noteA;
    }

    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setNoteB(const NoteNumber noteNumber)
{
    noteB = std::clamp(noteNumber, MinNoteNumber, MaxNoteNumber);

    if (noteB < noteA)
    {
        noteA = noteB;
    }

    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setControl(const int i)
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

void StepEditorScreen::setFromNote(const DrumNoteNumber noteNumber)
{
    fromNote = std::clamp(noteNumber, NoDrumNoteAssigned, MaxDrumNoteNumber);

    setViewNotesText();
    displayView();
    updateComponents();
    setViewNotesText();
    refreshEventRows();
    refreshSelection();
}

void StepEditorScreen::setSelectionStartIndex(const int i)
{
    if (const auto eventsAtCurrentTick = computeEventsAtCurrentTick();
        std::dynamic_pointer_cast<EmptyEvent>(eventsAtCurrentTick[i]))
    {
        return;
    }

    selectionStartIndex = i;
    selectionEndIndex = i;

    ls.lock()->setFunctionKeysArrangement(1);
    refreshSelection();
}

void StepEditorScreen::clearSelection()
{
    selectionStartIndex = -1;
    selectionEndIndex = -1;
    ls.lock()->setFunctionKeysArrangement(0);
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
    const std::string focus = ls.lock()->getFocusedFieldName();

    if (focus.length() != 2)
    {
        clearSelection();
        return;
    }

    const int row = stoi(focus.substr(1, 1));
    const int eventIndex = row + yOffset;

    if (selectionStartIndex > selectionEndIndex)
    {
        std::swap(selectionStartIndex, selectionEndIndex);
    }

    if (eventIndex < selectionStartIndex || eventIndex > selectionEndIndex)
    {
        clearSelection();
    }
}

void StepEditorScreen::setSelectedEvent(const std::weak_ptr<EventRef> &event)
{
    selectedEvent = event.lock();
}

void StepEditorScreen::setSelectedParameterLetter(const std::string &str)
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
            if (auto event = eventsAtCurrentTick[i];
                !std::dynamic_pointer_cast<EmptyEvent>(event))
            {
                const auto track = sequencer.lock()->getSelectedTrack();
                track->removeEvent(event);
            }
        }
    }

    clearSelection();
    setyOffset(0);
}

void StepEditorScreen::displayView() const
{
    findField("view")->setText(viewNames[view]);
}

std::vector<std::shared_ptr<EventRef>> &StepEditorScreen::getSelectedEvents()
{
    return selectedEvents;
}

std::string StepEditorScreen::getSelectedParameterLetter()
{
    return selectedParameterLetter;
}

std::shared_ptr<EventRef> StepEditorScreen::getSelectedEvent()
{
    return selectedEvent;
}

std::vector<std::shared_ptr<EventRef>> &StepEditorScreen::getPlaceHolder()
{
    return placeHolder;
}

int StepEditorScreen::getYOffset() const
{
    return yOffset;
}

void StepEditorScreen::adhocPlayNoteEvent(const EventData &noteEvent) const
{
    const auto track = sequencer.lock()->getSelectedTrack();
    const auto trackIndex = track->getIndex();
    const auto trackVelocityRatio = track->getVelocityRatio();
    const auto trackBusType = track->getBusType();
    const auto trackDeviceIndex = track->getDeviceIndex();

    utils::Task audioTask;
    audioTask.set(
        [this, e = noteEvent, trackIndex, trackVelocityRatio, trackBusType,
         trackDeviceIndex]
        {
            mpc.getEventHandler()->handleFinalizedEvent(
                e, trackIndex, trackVelocityRatio, trackBusType,
                trackDeviceIndex);
        });
    mpc.getEngineHost()->postToAudioThread(std::move(audioTask));
}

void StepEditorScreen::resetYPosAndYOffset()
{
    yOffset = 0;

    if (getActiveRow() <= 0)
    {
        return;
    }

    ls.lock()->setFocus("a0");
}

std::string StepEditorScreen::getActiveColumn() const
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.size() != 2)
    {
        return "";
    }

    return focusedFieldName.substr(0, 1);
}

int StepEditorScreen::getActiveRow() const
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

    ls.lock()->setFocus(desiredColumn + std::to_string(row));
}

void StepEditorScreen::adhocPlayNoteEventsAtCurrentPosition() const
{
    for (auto &e : computeEventsAtCurrentTick())
    {
        if (const auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(e))
        {
            adhocPlayNoteEvent(*noteEvent->handle);
        }
    }
}
