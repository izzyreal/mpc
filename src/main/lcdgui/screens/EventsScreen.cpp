#include "EventsScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lcdgui/screens/UserScreen.hpp>
#include "lcdgui/screens/SequencerScreen.hpp"

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

EventsScreen::EventsScreen(mpc::Mpc& mpc, const int layerIndex)
    : ScreenComponent(mpc, "events", layerIndex)
{
}

void EventsScreen::setNote0(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "note0")
    {
        WithTimesAndNotes::setNote0(i);
    }
}

void EventsScreen::open()
{
    sequencer.lock()->move(0);

    auto note1Field = findField("note1");

    if (setNote1X)
    {
        note1Field->setLocation(note1Field->getX() + 1, note1Field->getY());
        setNote1X = false;
    }

    note1Field->setSize(47, 9);
    note1Field->setAlignment(Alignment::Centered, 18);

    if (tab != 0)
    {
        mpc.getLayeredScreen()->openScreen(tabNames[tab]);
        return;
    }

    auto previousScreenWasSequencer = ls->isPreviousScreen<SequencerScreen>();
    auto seq = sequencer.lock()->getActiveSequence();

    if (previousScreenWasSequencer)
    {
        setFromTr(sequencer.lock()->getActiveTrackIndex());
        setToTr(sequencer.lock()->getActiveTrackIndex());

        if (!seq->isUsed())
        {
            auto userScreen = mpc.screens->get<UserScreen>();
            seq->init(userScreen->lastBar);
        }

        setToSq(sequencer.lock()->getActiveSequenceIndex());
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

        auto toSeqLastTick = sequencer.lock()->getSequence(toSq)->getLastTick();

        if (start > toSeqLastTick)
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

void EventsScreen::function(int i)
{

    auto fromSequence = sequencer.lock()->getActiveSequence();
    auto toSequence = sequencer.lock()->getSequence(toSq);

    switch (i)
    {
        // Intentional fall-through
        case 1:
        case 2:
        case 3:
            tab = i;
            mpc.getLayeredScreen()->openScreen(tabNames[tab]);
            break;
        case 5:
            {
                auto sourceTrack = sequencer.lock()->getActiveTrack();

                if (editFunctionNumber == 0)
                {
                    performCopy(time0, time1, toSq, start, toTr, modeMerge, copies, note0, note1);
                }
                else if (editFunctionNumber == 1)
                {
                    for (auto& noteEvent : sourceTrack->getNoteEvents()) {
                        if (durationMode == 0) {
                            noteEvent->setDuration(noteEvent->getDuration() + durationValue);
                        } else if (durationMode == 1) {
                            noteEvent->setDuration(noteEvent->getDuration() - durationValue);
                        } else if (durationMode == 2) {
                            noteEvent->setDuration(noteEvent->getDuration() * durationValue * 0.01);
                        } else if (durationMode == 3) {
                            noteEvent->setDuration(durationValue);
                        }
                    }
                }
                else if (editFunctionNumber == 2)
                {
                    for (auto& n : sourceTrack->getNoteEvents()) {
                        if (velocityMode == 0) {
                            n->setVelocity(n->getVelocity() + velocityValue);
                        } else if (velocityMode == 1) {
                            n->setVelocity(n->getVelocity() - velocityValue);
                        } else if (velocityMode == 2) {
                            n->setVelocity((n->getVelocity() * velocityValue * 0.01));
                        } else if (velocityMode == 3) {
                            n->setVelocity(velocityValue);
                        }
                    }
                }
                else if (editFunctionNumber == 3)
                {
                    // The original does not process DRUM tracks.
                    // We do, because it's nice and doesn't bother anyone,
                    // so you won't see any filtering of that kind here.
                    for (auto& n : sourceTrack->getNoteEvents())
                    {
                        n->setNote(n->getNote() + transposeAmount);
                    }
                }

                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            }
            break;
    }
}

void EventsScreen::turnWheel(int i)
{
    auto toSequence = sequencer.lock()->getSequence(toSq);

    if (checkAllTimesAndNotes(mpc, i, sequencer.lock()->getActiveSequence().get(), sequencer.lock()->getActiveTrack().get()))
    {
        return;
    }
    
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "start0")
    {
        setStart(SeqUtil::setBar(SeqUtil::getBar(toSequence.get(), start) + i, toSequence.get(), start));
    }
    else if (focusedFieldName == "start1")
    {
        setStart(SeqUtil::setBeat(SeqUtil::getBeat(toSequence.get(), start) + i, toSequence.get(), start));
    }
    else if (focusedFieldName == "start2")
    {
        setStart(SeqUtil::setClock(SeqUtil::getClock(toSequence.get(), start) + i, toSequence.get(), start));
    }
    else if (focusedFieldName == "edit")
    {
        setEdit(editFunctionNumber + i);
    }
    else if (focusedFieldName == "from-sq")
    {
        setFromSq(sequencer.lock()->getActiveSequenceIndex() + i);

        auto fromSeq = sequencer.lock()->getActiveSequence();

        if (time1 > fromSeq->getLastTick())
            setTime1(fromSeq->getLastTick());
    }
    else if (focusedFieldName == "from-tr")
    {
        setFromTr(sequencer.lock()->getActiveTrackIndex() +i);
    }
    else if (focusedFieldName == "to-sq")
    {
        setToSq(toSq + i);
        auto toSeq = sequencer.lock()->getSequence(toSq);

        if (start > toSeq->getLastTick())
            setStart(toSeq->getLastTick());
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

void EventsScreen::displayStart()
{
    auto seq = sequencer.lock()->getSequence(toSq);
    findField("start0")->setTextPadded(SeqUtil::getBar(seq.get(), start) + 1, "0");
    findField("start1")->setTextPadded(SeqUtil::getBeat(seq.get(), start) + 1, "0");
    findField("start2")->setTextPadded(SeqUtil::getClock(seq.get(), start), "0");
}

void EventsScreen::displayTime()
{
    auto seq = sequencer.lock()->getActiveSequence();
    findField("time0")->setTextPadded(SeqUtil::getBar(seq.get(), time0) + 1, "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(seq.get(), time0) + 1, "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(seq.get(), time0), "0");
    findField("time3")->setTextPadded(SeqUtil::getBar(seq.get(), time1) + 1, "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(seq.get(), time1) + 1, "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(seq.get(), time1), "0");
}

void EventsScreen::displayCopies()
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

void EventsScreen::displayMode()
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
            findField("mode")->setTextPadded("+" + std::to_string(transposeAmount));
        }
    }
}

void EventsScreen::displayEdit()
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

    if (sequencer.lock()->getActiveTrack()->getBus() == 0)
    {
        findField("note0")->setSize(47, 9);
        findField("note1")->Hide(false);
        findLabel("note1")->Hide(false);
        findField("note0")->setAlignment(Alignment::Centered, 18);
        displayMidiNotes();
    }
    else
    {
        findField("note0")->setSize(37, 9);
        findField("note1")->Hide(true);
        findLabel("note1")->Hide(true);
        findField("note0")->setAlignment(Alignment::None);
        displayDrumNotes();
    }
}

void EventsScreen::displayMidiNotes()
{
    findField("note0")->setText(StrUtil::padLeft(std::to_string(note0), " ", 3) + "(" + mpc::Util::noteNames()[note0] + u8"\u00D4");
    findField("note1")->setText(StrUtil::padLeft(std::to_string(note1), " ", 3) + "(" + mpc::Util::noteNames()[note1] + u8"\u00D4");
}

void EventsScreen::displayDrumNotes()
{
    if (note0 == 34)
    {
        findField("note0")->setText("ALL");
    }
    else
    {
        auto track = sequencer.lock()->getActiveTrack();
        auto program = sampler->getProgram(mpc.getDrum(track->getBus() - 1).getProgram());

        auto noteText = StrUtil::padLeft(std::to_string(note0), " ", 2);
        auto padName = sampler->getPadName(program->getPadIndexFromNote(note0));
        findField("note0")->setText(noteText + "/" + padName);
    }
}

void EventsScreen::setEdit(int i)
{
    if (i < 0 || i > 3)
        return;

    editFunctionNumber = i;
    displayEdit();
}

void EventsScreen::setFromSq(int i)
{
    if (i < 0 || i > 98)
        return;

    sequencer.lock()->setActiveSequenceIndex(i);

    displayFromSq();
}

void EventsScreen::setFromTr(int i)
{
    if (i < 0 || i > 63)
        return;

    sequencer.lock()->setActiveTrackIndex(i);

    displayFromTr();
}

void EventsScreen::setToSq(int i)
{
    if (i < 0 || i > 98)
        return;

    toSq = i;
    displayToSq();
}

void EventsScreen::setToTr(int i)
{
    if (i < 0 || i > 63)
        return;

    toTr = i;
    displayToTr();
}

void EventsScreen::setModeMerge(bool b)
{
    modeMerge = b;
    displayMode();
}

void EventsScreen::setCopies(int i)
{
    if (i < 1 || i > 999)
        return;

    copies = i;
    displayCopies();
}

void EventsScreen::setDurationMode(int i)
{
    if (i < 0 || i > 3)
        return;

    durationMode = i;

    if (durationMode == 2 && durationValue > 200)
        setDuration(200);

    displayMode();
}

void EventsScreen::setVelocityMode(int i)
{
    if (i < 0 || i > 3)
        return;

    velocityMode = i;

    if (velocityMode != 2 && velocityValue > 127)
        setVelocityValue(127);

    displayMode();
}

void EventsScreen::setTransposeAmount(int i)
{
    if (i < -12 || i > 12)
        return;

    transposeAmount = i;
    // Field otherwise used for displaying mode is
    // replaced by an "Amount:" field.
    displayMode();
}

void EventsScreen::setDuration(int i)
{
    if (i < 1 || i > 9999)
        return;

    if (durationMode == 2 && i > 200)
        return;

    durationValue = i;
    displayCopies();
}

void EventsScreen::setVelocityValue(int i)
{
    if (i < 1 || i > 200)
        return;

    if (velocityMode != 2 && i > 127)
        return;

    velocityValue = i;

    // Field otherwise used for displaying "Copies:" is
    // replaced by a "Value:" field.
    displayCopies();
}

void EventsScreen::setStart(int i)
{
    start = i;
    displayStart();
}

void EventsScreen::displayFromSq()
{
    findField("from-sq")->setTextPadded(sequencer.lock()->getActiveSequenceIndex() + 1);
}

void EventsScreen::displayFromTr()
{
    findField("from-tr")->setTextPadded(sequencer.lock()->getActiveTrackIndex() + 1);
}

void EventsScreen::displayToSq()
{
    findField("to-sq")->setTextPadded(toSq + 1);
}

void EventsScreen::displayToTr()
{
    findField("to-tr")->setTextPadded(toTr + 1);
}

void EventsScreen::performCopy(int sourceStart, int sourceEnd, int toSequenceIndex, int destStart,
        int toTrackIndex, bool copyModeMerge, int copyCount, int copyNote0, int copyNote1)
{
    const auto segLength = sourceEnd - sourceStart;
    auto sourceTrack = sequencer.lock()->getActiveTrack();

    auto fromSequence = sequencer.lock()->getActiveSequence();

    if (!fromSequence->isUsed())
    {
        return;
    }

    auto destOffset = destStart - sourceStart;

    auto toSequence = sequencer.lock()->getSequence(toSequenceIndex);

    if (!toSequence->isUsed())
        toSequence->init(fromSequence->getLastBarIndex());

    auto destNumerator = -1;
    auto destDenominator = -1;
    auto destBarLength = -1;

    for (int i = 0; i <= toSequence->getLastBarIndex(); i++)
    {
        auto firstTickOfBar = toSequence->getFirstTickOfBar(i);
        auto barLength = toSequence->getBarLengthsInTicks()[i];
        if (destStart >= firstTickOfBar &&
                destStart <= firstTickOfBar + barLength)
        {
            destNumerator = toSequence->getNumerator(i);
            destDenominator = toSequence->getDenominator(i);
            destBarLength = barLength;
            break;
        }
    }

    auto minimumRequiredNewSequenceLength = destStart + (segLength);
    auto ticksToAdd = minimumRequiredNewSequenceLength - toSequence->getLastTick();
    auto barsToAdd = (int) (ceil((float)ticksToAdd / destBarLength));
    auto initialLastBarIndex = toSequence->getLastBarIndex();
    for (int i = 0; i < barsToAdd; i++)
    {
        const auto afterBar = initialLastBarIndex + i + 1;

        if (afterBar >= 998)
            break;

        toSequence->insertBars(1, afterBar);
        toSequence->setTimeSignature(afterBar, destNumerator, destDenominator);
    }

    auto destTrack = toSequence->getTrack(toTrackIndex);

    if (!copyModeMerge)
    {
        auto destTrackEvents = destTrack->getEvents();
        for (auto& e : destTrackEvents)
        {
            auto tick = e->getTick();

            if (tick >= destOffset && tick < destOffset + (segLength * copyCount))
                destTrack->removeEvent(e);
        }
    }

    auto sourceTrackEvents = sourceTrack->getEvents();

    for (auto& e : sourceTrackEvents)
    {
        auto ne = std::dynamic_pointer_cast<NoteOnEvent>(e);

        if (ne)
        {
            if (sourceTrack->getBus() == 0)
            {
                if (ne->getNote() < copyNote0 || ne->getNote() > copyNote1)
                    continue;
            }
            else
            {
                if (copyNote0 != 34 && copyNote0 != ne->getNote())
                    continue;
            }
        }

        if (e->getTick() >= sourceEnd)
            break;

        if (e->getTick() >= sourceStart)
        {
            for (int copy = 0; copy < copyCount; copy++)
            {
                int tickCandidate = e->getTick() + destOffset + (copy * segLength);

                if (tickCandidate >= toSequence->getLastTick())
                    break;

                destTrack->cloneEventIntoTrack(e, tickCandidate);
            }
        }
    }

    sequencer.lock()->setActiveSequenceIndex(toSequenceIndex);
}
