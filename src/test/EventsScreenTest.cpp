#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"

#include "sequencer/EventRef.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

TEST_CASE("COPY1", "[events-screen]")
{
    /**
     * Before: 1 bar, 4/4, a note on the first 8 16ths.
     * Copy operation: Copy all notes of the first half of the bar to the second
     * half. After: a note on all 16ths.
     */
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();

    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 192;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, SequenceIndex(toSeq), destStart,
                              toTrack, merge, copies, note0, note1);

    stateManager->drainQueue();

    REQUIRE(tr->getEvents().size() == 16);

    tickPos = 0;

    for (int i = 0; i < 16; i++)
    {
        REQUIRE(tr->getEvent(i)->getTick() == tickPos);
        REQUIRE(tr->getNoteEvents()[i]->getNote() == 35 + (i % 8));
        tickPos += 24;
    }
}

TEST_CASE("COPY2", "[events-screen]")
{
    /**
     * Before: 1 bar, 4/4, a note on the first 8 16ths, with 23 ticks "delay".
     * Copy operation: Copy all notes of the first half of the bar to the second
     * half. After: a note on all 16ths with 23 ticks "delay".
     */
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    int tickPos = 23;

    for (int i = 0; i < 8; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 192;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, SequenceIndex(toSeq), destStart,
                              toTrack, merge, copies, note0, note1);

    stateManager->drainQueue();

    REQUIRE(tr->getEvents().size() == 16);

    tickPos = 23;

    for (int i = 0; i < 16; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(tr->getEvent(i));
        REQUIRE(noteEvent->getTick() == tickPos);
        REQUIRE(noteEvent->getNote() == 35 + (i % 8));
        tickPos += 24;
    }
}

TEST_CASE("COPY3", "[events-screen]")
{
    /**
     * Before: 1 bar, 4/4, a note on the first 8 16ths, with 23 ticks "delay".
     * Copy operation: Copy all notes of the first half of the bar to 1 tick
     *                 after the start of the second half.
     * After: 16 notes on all 16ths with 23 ticks "delay". Now the sequence is 2
     *        bars long, because the 2 delays added up imply bleeding outside
     *        the end of the first bar.
     */
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    int tickPos = 23;

    for (int i = 0; i < 8; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    assert(tr->getEvents().size() == 8);

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 193;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, SequenceIndex(toSeq), destStart,
                              toTrack, merge, copies, note0, note1);

    stateManager->drainQueue();

    REQUIRE(tr->getEvents().size() == 16);

    tickPos = 23;

    for (int i = 0; i < 15; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(tr->getEvent(i));
        REQUIRE(noteEvent->getTick() == tickPos);
        REQUIRE(noteEvent->getNote() == 35 + (i % 8));
        tickPos += 24;
        if (i == 7)
        {
            tickPos++;
        }
    }
}

TEST_CASE("COPY4", "[events-screen]")
{
    /**
     * Let's say we have 2 empty bars: first bar is 4/4, second bar is 3/4.
     * We copy 001.01.00 to 002.01.00 to 003.01.00.
     * There is not enough space in the sequence for a 3rd bar yet.
     * This 3rd bar will be inserted, and it will get the same time signature
     * as the preceding bar, so 3/4. This still leaves 1/4th of a source bar
     * that needs to be copied, so another bar of 3/4 is inserted.
     */

    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(1);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    seq->setTimeSignature(1, 3, 4);
    stateManager->drainQueue();

    assert(seq->getLastTick() == 384 + 288);

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    int start = 0;
    int end = 384;
    int toSeq = 0;
    int destStart = 384 + 288;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, SequenceIndex(toSeq), destStart,
                              toTrack, merge, copies, note0, note1);

    stateManager->drainQueue();

    auto seqSnapshot = stateManager->getSnapshot().getSequenceState(
        stateManager->getSnapshot().getSelectedSequenceIndex());

    REQUIRE(seqSnapshot.getLastTick() == 384 + 288 + 288 + 288);
    REQUIRE(seqSnapshot.getBarLength(0) == 384);
    REQUIRE(seqSnapshot.getBarLength(1) == 288);
    REQUIRE(seqSnapshot.getBarLength(2) == 288);
    REQUIRE(seqSnapshot.getBarLength(3) == 288);
}

TEST_CASE("COPY5", "[events-screen]")
{
    /**
     * Before: A note with note noteIndex + 35 at every 16th
     * Copy operation: Replace 2nd half of the bar with the first
     * After: A note with note noteIndex + 35 at every 16th, resetting noteIndex
     * to 0 at note 8
     */
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 16; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 192;
    int toTrack = 0;
    bool merge = false;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, SequenceIndex(toSeq), destStart,
                              toTrack, merge, copies, note0, note1);

    stateManager->drainQueue();

    REQUIRE(tr->getEvents().size() == 16);

    tickPos = 0;

    for (int i = 0; i < 16; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(tr->getEvent(i));
        REQUIRE(noteEvent->getTick() == tickPos);
        REQUIRE(noteEvent->getNote() == 35 + (i % 8));
        tickPos += 24;
    }
}

TEST_CASE("COPY6", "[events-screen]")
{
    /**
     * Before: A note with note noteIndex + 35 at every 16th
     * Copy operation: Merge first half of the bar into the second
     * After: 24 notes in total. The first half repeats in the second half,
     *        while the original 8 notes of the 2nd half are still there.
     */
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 16; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 192;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, SequenceIndex(toSeq), destStart,
                              toTrack, merge, copies, note0, note1);

    stateManager->drainQueue();

    REQUIRE(tr->getEvents().size() == 24);

    tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(tr->getEvent(i));
        REQUIRE(noteEvent->getTick() == tickPos);
        REQUIRE(noteEvent->getNote() == 35 + i);
        tickPos += 24;
    }

    int eventIndex = 8;

    for (int i = 8; i < 16; i++)
    {
        auto e1 =
            std::dynamic_pointer_cast<NoteOnEvent>(tr->getEvent(eventIndex++));
        auto e2 =
            std::dynamic_pointer_cast<NoteOnEvent>(tr->getEvent(eventIndex++));
        REQUIRE(e1->getTick() == tickPos);
        REQUIRE(e1->getNote() == 35 + i);
        REQUIRE(e2->getTick() == tickPos);
        REQUIRE(e2->getNote() == 27 + i);
        tickPos += 24;
    }
}
