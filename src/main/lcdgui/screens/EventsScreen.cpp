#include "EventsScreen.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "MpcSpecs.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/SeqUtil.hpp"

#include "lcdgui/screens/UserScreen.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

EventsScreen::EventsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "events", layerIndex)
{
}

void EventsScreen::setNote0(const NoteNumber noteNumber)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "note0")
    {
        WithTimesAndNotes::setNote0(noteNumber);
    }
}

void EventsScreen::open()
{
    sequencer->getTransport()->setPosition(0);

    const auto note1Field = findField("note1");

    if (setNote1X)
    {
        note1Field->setLocation(note1Field->getX() + 1, note1Field->getY());
        setNote1X = false;
    }

    note1Field->setSize(47, 9);
    note1Field->setAlignment(Alignment::Centered, 18);

    if (tab != 0)
    {
        ls->openScreen(tabNames[tab]);
        return;
    }

    const auto previousScreenWasSequencer =
        ls->isPreviousScreen({ScreenId::SequencerScreen});
    const auto seq = sequencer->getActiveSequence();

    if (previousScreenWasSequencer)
    {
        setFromTr(sequencer->getActiveTrackIndex());
        setToTr(sequencer->getActiveTrackIndex());

        if (!seq->isUsed())
        {
            const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
            seq->init(userScreen->lastBar);
        }

        setToSq(sequencer->getActiveSequenceIndex());
        time0 = 0;
        time1 = seq->getLastTick();
        start = 0;
    }
    else
    {
        if (time0 > seq->getLastTick())
        {
            time0 = 0;
        }

        if (time1 > seq->getLastTick())
        {
            time1 = 0;
        }

        if (const auto toSeqLastTick =
                sequencer->getSequence(toSq)->getLastTick();
            start > toSeqLastTick)
        {
            start = toSeqLastTick;
        }
    }

    displayFromSq();
    displayTime();
    displayEdit();
    displayNotes();
    displayMode();
    displayStart();
    displayCopies();
}

void EventsScreen::function(const int i)
{

    auto fromSequence = sequencer->getActiveSequence();
    auto toSequence = sequencer->getSequence(toSq);

    switch (i)
    {
        // Intentional fall-through
        case 1:
        case 2:
        case 3:
            tab = i;
            ls->openScreen(tabNames[tab]);
            break;
        case 5:
        {
            const auto sourceTrack = sequencer->getActiveTrack();

            if (editFunctionNumber == 0)
            {
                performCopy(time0, time1, toSq, start, toTr, modeMerge, copies,
                            note0, note1);
            }
            else if (editFunctionNumber == 1)
            {
                for (const auto &noteEvent : sourceTrack->getNoteEvents())
                {
                    if (durationMode == 0)
                    {
                        noteEvent->setDuration(noteEvent->getDuration() +
                                               durationValue);
                    }
                    else if (durationMode == 1)
                    {
                        noteEvent->setDuration(noteEvent->getDuration() -
                                               durationValue);
                    }
                    else if (durationMode == 2)
                    {
                        noteEvent->setDuration(noteEvent->getDuration() *
                                               durationValue * 0.01);
                    }
                    else if (durationMode == 3)
                    {
                        noteEvent->setDuration(durationValue);
                    }
                }
            }
            else if (editFunctionNumber == 2)
            {
                for (const auto &n : sourceTrack->getNoteEvents())
                {
                    if (velocityMode == 0)
                    {
                        n->setVelocity(n->getVelocity() + velocityValue);
                    }
                    else if (velocityMode == 1)
                    {
                        n->setVelocity(n->getVelocity() - velocityValue);
                    }
                    else if (velocityMode == 2)
                    {
                        n->setVelocity(n->getVelocity() *
                                       Velocity(velocityValue) * 0.01);
                    }
                    else if (velocityMode == 3)
                    {
                        n->setVelocity(Velocity(velocityValue));
                    }
                }
            }
            else if (editFunctionNumber == 3)
            {
                // The original does not process DRUM tracks.
                // We do, because it's nice and doesn't bother anyone,
                // so you won't see any filtering of that kind here.
                for (const auto &n : sourceTrack->getNoteEvents())
                {
                    n->setNote(n->getNote() + transposeAmount);
                }
            }

            openScreenById(ScreenId::SequencerScreen);
        }
        break;
        default:;
    }
}

void EventsScreen::turnWheel(const int i)
{
    const auto toSequence = sequencer->getSequence(toSq);

    if (checkAllTimesAndNotes(mpc, i, sequencer->getActiveSequence().get(),
                              sequencer->getActiveTrack().get()))
    {
        return;
    }

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "start0")
    {
        setStart(SeqUtil::setBar(SeqUtil::getBar(toSequence.get(), start) + i,
                                 toSequence.get(), start));
    }
    else if (focusedFieldName == "start1")
    {
        setStart(SeqUtil::setBeat(SeqUtil::getBeat(toSequence.get(), start) + i,
                                  toSequence.get(), start));
    }
    else if (focusedFieldName == "start2")
    {
        setStart(
            SeqUtil::setClock(SeqUtil::getClock(toSequence.get(), start) + i,
                              toSequence.get(), start));
    }
    else if (focusedFieldName == "edit")
    {
        setEdit(editFunctionNumber + i);
    }
    else if (focusedFieldName == "from-sq")
    {
        setFromSq(sequencer->getActiveSequenceIndex() + i);

        if (const auto fromSeq = sequencer->getActiveSequence();
            time1 > fromSeq->getLastTick())
        {
            setTime1(fromSeq->getLastTick());
        }
    }
    else if (focusedFieldName == "from-tr")
    {
        setFromTr(sequencer->getActiveTrackIndex() + i);
    }
    else if (focusedFieldName == "to-sq")
    {
        setToSq(toSq + i);

        if (const auto toSeq = sequencer->getSequence(toSq);
            start > toSeq->getLastTick())
        {
            setStart(toSeq->getLastTick());
        }
    }
    else if (focusedFieldName == "to-tr")
    {
        setToTr(toTr + i);
    }
    else if (focusedFieldName == "mode")
    {
        if (editFunctionNumber == 0)
        {
            setModeMerge(i > 0);
        }
        else if (editFunctionNumber == 1)
        {
            setDurationMode(durationMode + i);
        }
        else if (editFunctionNumber == 2)
        {
            setVelocityMode(velocityMode + i);
        }
        else if (editFunctionNumber == 3)
        {
            setTransposeAmount(transposeAmount + i);
        }
    }
    else if (focusedFieldName == "copies")
    {
        if (editFunctionNumber == 0)
        {
            setCopies(copies + i);
        }
        else if (editFunctionNumber == 1)
        {
            setDuration(durationValue + i);
        }
        else if (editFunctionNumber == 2)
        {
            setVelocityValue(velocityValue + i);
        }
    }
}

void EventsScreen::displayStart() const
{
    const auto seq = sequencer->getSequence(toSq);
    findField("start0")->setTextPadded(SeqUtil::getBar(seq.get(), start) + 1,
                                       "0");
    findField("start1")->setTextPadded(SeqUtil::getBeat(seq.get(), start) + 1,
                                       "0");
    findField("start2")->setTextPadded(SeqUtil::getClock(seq.get(), start),
                                       "0");
}

void EventsScreen::displayTime()
{
    const auto seq = sequencer->getActiveSequence();
    findField("time0")->setTextPadded(SeqUtil::getBar(seq.get(), time0) + 1,
                                      "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(seq.get(), time0) + 1,
                                      "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(seq.get(), time0), "0");
    findField("time3")->setTextPadded(SeqUtil::getBar(seq.get(), time1) + 1,
                                      "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(seq.get(), time1) + 1,
                                      "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(seq.get(), time1), "0");
}

void EventsScreen::displayCopies() const
{
    if (editFunctionNumber == 0)
    {
        findField("copies")->setTextPadded(copies);
    }
    else if (editFunctionNumber == 1)
    {
        findField("copies")->setTextPadded(durationValue);
    }
    else if (editFunctionNumber == 2)
    {
        findField("copies")->setTextPadded(velocityValue);
    }
}

void EventsScreen::displayMode() const
{
    if (editFunctionNumber == 0)
    {
        findField("mode")->setText(modeMerge ? "MERGE" : "REPLACE");
    }
    if (editFunctionNumber == 1)
    {
        findField("mode")->setText(modeNames[durationMode]);
    }
    if (editFunctionNumber == 2)
    {
        findField("mode")->setText(modeNames[velocityMode]);
    }
    if (editFunctionNumber == 3)
    {
        if (transposeAmount == 0)
        {
            findField("mode")->setTextPadded(0);
        }
        else if (transposeAmount < 0)
        {
            findField("mode")->setTextPadded(transposeAmount);
        }
        else
        {
            findField("mode")->setTextPadded("+" +
                                             std::to_string(transposeAmount));
        }
    }
}

void EventsScreen::displayEdit() const
{
    findField("edit")->setText(functionNames[editFunctionNumber]);

    if (editFunctionNumber == 0)
    {
        findLabel("from-sq")->setLocation(132, 1);
        findField("from-sq")->setLocation(findField("from-sq")->getX(), 1);
        findLabel("from-tr")->setLocation(findLabel("from-tr")->getX(), 1);
        findField("from-tr")->setLocation(findField("from-tr")->getX(), 1);
        findLabel("mode")->setText("Mode:");
        findLabel("from-sq")->setText("From sq:");
        findField("to-sq")->Hide(false);
        findField("to-tr")->Hide(false);
        findField("start0")->Hide(false);
        findField("start1")->Hide(false);
        findField("start2")->Hide(false);
        findField("copies")->Hide(false);
        findLabel("to-sq")->Hide(false);
        findLabel("to-tr")->Hide(false);
        findLabel("start0")->Hide(false);
        findLabel("start1")->Hide(false);
        findLabel("start2")->Hide(false);
        findLabel("copies")->setText("Copies:");
        findLabel("copies")->setSize(7 * 6 + 1, 7);
        findLabel("copies")->setLocation(138, 39);
        findField("copies")->setLocation(findField("copies")->getX(), 38);
        findLabel("mode")->setLocation(150, 20);
        findField("mode")->setSize(7 * 6 + 1, 9);
        findField("copies")->setSize(3 * 6 + 1, 9);
    }
    else if (editFunctionNumber == 1 || editFunctionNumber == 2)
    {
        findLabel("from-sq")->setLocation(132, 3);
        findField("from-sq")->setLocation(findField("from-sq")->getX(), 3);
        findLabel("from-tr")->setLocation(findLabel("from-tr")->getX(), 3);
        findField("from-tr")->setLocation(findField("from-tr")->getX(), 3);
        findLabel("mode")->setText("Mode:");
        findLabel("from-sq")->setText("Edit sq:");
        findField("to-sq")->Hide(true);
        findField("to-tr")->Hide(true);
        findField("start0")->Hide(true);
        findField("start1")->Hide(true);
        findField("start2")->Hide(true);
        findField("copies")->Hide(false);
        findLabel("to-sq")->Hide(true);
        findLabel("to-tr")->Hide(true);
        findLabel("start0")->Hide(true);
        findLabel("start1")->Hide(true);
        findLabel("start2")->Hide(true);
        findLabel("copies")->setText("Value:");
        findLabel("copies")->setSize(6 * 6 + 1, 7);
        findLabel("copies")->setLocation(144, 35);
        findField("copies")->setLocation(findField("copies")->getX(), 34);

        if (editFunctionNumber == 2)
        {
            findField("copies")->setSize(3 * 6 + 1, 9);
        }
        else
        {
            findField("copies")->setSize(4 * 6 + 1, 9);
        }
        findLabel("mode")->setLocation(150, 20);
        findField("mode")->setSize(10 * 6 + 1, 9);
    }
    else if (editFunctionNumber == 3)
    {
        findLabel("from-sq")->setLocation(132, 3);
        findField("from-sq")->setLocation(findField("from-sq")->getX(), 3);
        findLabel("from-tr")->setLocation(findLabel("from-tr")->getX(), 3);
        findField("from-tr")->setLocation(findField("from-tr")->getX(), 3);
        findLabel("mode")->setText("Amount:");
        findLabel("from-sq")->setText("Edit sq:");
        findField("to-sq")->Hide(true);
        findField("to-tr")->Hide(true);
        findField("start0")->Hide(true);
        findField("start1")->Hide(true);
        findField("start2")->Hide(true);
        findField("copies")->Hide(true);
        findLabel("to-sq")->Hide(true);
        findLabel("to-tr")->Hide(true);
        findLabel("start0")->Hide(true);
        findLabel("start1")->Hide(true);
        findLabel("start2")->Hide(true);
        findLabel("copies")->setText("(Except drum track)");
        findLabel("copies")->setLocation(132, 38);
        findLabel("copies")->setSize(113, 7);
        findLabel("mode")->setLocation(138, 20);
        findField("mode")->setSize(3 * 6 + 1, 9);
    }

    displayCopies();
    displayFromSq();
    displayFromTr();
    displayMode();
    displayStart();
    displayToSq();
    displayToTr();
}

void EventsScreen::displayNotes()
{
    if (isDrumBusType(sequencer->getActiveTrack()->getBusType()))
    {
        findField("note0")->setSize(37, 9);
        findField("note1")->Hide(true);
        findLabel("note1")->Hide(true);
        findField("note0")->setAlignment(Alignment::None);
        displayDrumNotes();
    }
    else
    {
        assert(isMidiBusType(sequencer->getActiveTrack()->getBusType()));
        findField("note0")->setSize(47, 9);
        findField("note1")->Hide(false);
        findLabel("note1")->Hide(false);
        findField("note0")->setAlignment(Alignment::Centered, 18);
        displayMidiNotes();
    }
}

void EventsScreen::displayMidiNotes() const
{
    findField("note0")->setText(
        StrUtil::padLeft(std::to_string(note0), " ", 3) + "(" +
        Util::noteNames()[note0] + u8"\u00D4");
    findField("note1")->setText(
        StrUtil::padLeft(std::to_string(note1), " ", 3) + "(" +
        Util::noteNames()[note1] + u8"\u00D4");
}

void EventsScreen::displayDrumNotes()
{
    if (note0 == NoDrumNoteAssigned)
    {
        findField("note0")->setText("ALL");
    }
    else
    {
        const auto track = sequencer->getActiveTrack();
        const auto drumBus = sequencer->getBus<DrumBus>(track->getBusType());
        assert(drumBus);
        const auto program = sampler->getProgram(drumBus->getProgram());

        const auto noteText = StrUtil::padLeft(std::to_string(note0), " ", 2);
        const auto padName = sampler->getPadName(
            program->getPadIndexFromNote(DrumNoteNumber(note0)));
        findField("note0")->setText(noteText + "/" + padName);
    }
}

void EventsScreen::setEdit(const int i)
{
    editFunctionNumber = std::clamp(i, 0, 3);
    displayEdit();
}

void EventsScreen::setFromSq(const SequenceIndex i) const
{
    sequencer->setActiveSequenceIndex(i, true);
    displayFromSq();
}

void EventsScreen::setFromTr(const int i) const
{
    sequencer->setActiveTrackIndex(
        std::clamp(i, 0, static_cast<int>(Mpc2000XlSpecs::LAST_TRACK_INDEX)));
    displayFromTr();
}

void EventsScreen::setToSq(const SequenceIndex i)
{
    toSq = std::clamp(
        i, MinSequenceIndex,
        static_cast<SequenceIndex>(Mpc2000XlSpecs::LAST_SEQUENCE_INDEX));
    displayToSq();
}

void EventsScreen::setToTr(const int i)
{
    toTr = std::clamp(i, 0, static_cast<int>(Mpc2000XlSpecs::LAST_TRACK_INDEX));
    displayToTr();
}

void EventsScreen::setModeMerge(const bool b)
{
    modeMerge = b;
    displayMode();
}

void EventsScreen::setCopies(const int i)
{
    copies = std::clamp(i, 1, 999);
    displayCopies();
}

void EventsScreen::setDurationMode(const int i)
{
    durationMode = std::clamp(i, 0, 3);

    if (durationMode == 2 && durationValue > 200)
    {
        setDuration(200);
    }

    displayMode();
}

void EventsScreen::setVelocityMode(const int i)
{
    velocityMode = std::clamp(i, 0, 3);

    if (velocityMode != 2 && velocityValue > MaxVelocity)
    {
        setVelocityValue(MaxVelocity);
    }

    displayMode();
}

void EventsScreen::setTransposeAmount(const int i)
{
    transposeAmount = std::clamp(i, -12, 12);
    // Field otherwise used for displaying mode is
    // replaced by an "Amount:" field.
    displayMode();
}

void EventsScreen::setDuration(const int i)
{
    durationValue = std::clamp(
        i, 1,
        durationMode == 2 ? 200 : Mpc2000XlSpecs::MAX_NOTE_EVENT_DURATION);
    displayCopies();
}

void EventsScreen::setVelocityValue(const int i)
{
    velocityValue = std::clamp(i, 1, velocityMode == 2 ? 200 : MaxVelocity);

    // Field otherwise used for displaying "Copies:" is
    // replaced by a "Value:" field.
    displayCopies();
}

void EventsScreen::setStart(const int startTick)
{
    start = startTick;
    displayStart();
}

void EventsScreen::displayFromSq() const
{
    findField("from-sq")->setTextPadded(
        sequencer->getActiveSequenceIndex().get() + 1);
}

void EventsScreen::displayFromTr() const
{
    findField("from-tr")->setTextPadded(sequencer->getActiveTrackIndex() + 1);
}

void EventsScreen::displayToSq() const
{
    findField("to-sq")->setTextPadded(toSq + 1);
}

void EventsScreen::displayToTr() const
{
    findField("to-tr")->setTextPadded(toTr + 1);
}

void EventsScreen::performCopy(const int sourceStart, const int sourceEnd,
                               const SequenceIndex toSequenceIndex,
                               const int destStart, const int toTrackIndex,
                               const bool copyModeMerge, const int copyCount,
                               const int copyNote0, const int copyNote1) const
{
    const auto segLength = sourceEnd - sourceStart;
    const auto sourceTrack = sequencer->getActiveTrack();

    const auto fromSequence = sequencer->getActiveSequence();

    if (!fromSequence->isUsed())
    {
        return;
    }

    const auto destOffset = destStart - sourceStart;

    const auto toSequence = sequencer->getSequence(toSequenceIndex);

    if (!toSequence->isUsed())
    {
        toSequence->init(fromSequence->getLastBarIndex());
    }

    auto destNumerator = -1;
    auto destDenominator = -1;
    auto destBarLength = -1;

    for (int i = 0; i <= toSequence->getLastBarIndex(); i++)
    {
        const auto firstTickOfBar = toSequence->getFirstTickOfBar(i);

        if (const auto barLength = toSequence->getBarLengthsInTicks()[i];
            destStart >= firstTickOfBar &&
            destStart <= firstTickOfBar + barLength)
        {
            destNumerator = toSequence->getNumerator(i);
            destDenominator = toSequence->getDenominator(i);
            destBarLength = barLength;
            break;
        }
    }

    const auto minimumRequiredNewSequenceLength = destStart + segLength;
    const auto ticksToAdd =
        minimumRequiredNewSequenceLength - toSequence->getLastTick();
    const auto barsToAdd =
        static_cast<int>(ceil(static_cast<float>(ticksToAdd) / destBarLength));
    const auto initialLastBarIndex = toSequence->getLastBarIndex();
    for (int i = 0; i < barsToAdd; i++)
    {
        const auto afterBar = initialLastBarIndex + i + 1;

        if (afterBar >= 998)
        {
            break;
        }

        toSequence->insertBars(1, afterBar);
        toSequence->setTimeSignature(afterBar, destNumerator, destDenominator);
    }

    const auto destTrack = toSequence->getTrack(toTrackIndex);

    if (!copyModeMerge)
    {
        const auto destTrackEvents = destTrack->getEvents();
        for (auto &e : destTrackEvents)
        {

            if (const auto tick = e->getTick();
                tick >= destOffset && tick < destOffset + segLength * copyCount)
            {
                destTrack->removeEvent(e);
            }
        }
    }

    const auto sourceTrackEvents = sourceTrack->getEvents();

    for (auto &e : sourceTrackEvents)
    {
        if (const auto ne = std::dynamic_pointer_cast<NoteOnEvent>(e))
        {
            if (isDrumBusType(sourceTrack->getBusType()))
            {
                if (copyNote0 != NoDrumNoteAssigned &&
                    copyNote0 != ne->getNote())
                {
                    continue;
                }
            }
            else
            {
                if (ne->getNote() < copyNote0 || ne->getNote() > copyNote1)
                {
                    continue;
                }
            }
        }

        if (e->getTick() >= sourceEnd)
        {
            break;
        }

        if (e->getTick() >= sourceStart)
        {
            for (int copy = 0; copy < copyCount; copy++)
            {
                const int tickCandidate =
                    e->getTick() + destOffset + copy * segLength;

                if (tickCandidate >= toSequence->getLastTick())
                {
                    break;
                }

                destTrack->cloneEventIntoTrack(e, tickCandidate);
            }
        }
    }

    sequencer->setActiveSequenceIndex(toSequenceIndex, true);
}
