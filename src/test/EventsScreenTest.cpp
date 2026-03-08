#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"
#include "lcdgui/Field.hpp"

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
    int note0 = AllDrumNotes;
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

TEST_CASE("COPY7 - time0 scroll in 8 bars of 2/4", "[events-screen]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();

    seq->init(7); // 8 bars.
    stateManager->drainQueue();
    seq->setTimeSignature(0, seq->getLastBarIndex(), 2, 4);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);
    mpc.getScreen()->function(1); // MAIN SCREEN > EDIT

    REQUIRE(layeredScreen->getCurrentScreenId() == ScreenId::EventsScreen);
    REQUIRE(layeredScreen->setFocus("time0"));

    auto controls = mpc.getScreen();
    auto barText = [&]
    {
        return controls->findChild<Field>("time0")->getText();
    };

    REQUIRE(barText() == "001");

    for (const auto &expectedBar :
         std::vector<std::string>{"002", "003", "004", "005",
                                  "006", "007", "008", "009"})
    {
        controls->turnWheel(1);
        REQUIRE(barText() == expectedBar);
    }

    controls->turnWheel(-1);
    REQUIRE(barText() == "008");
    controls->turnWheel(-1);
    REQUIRE(barText() == "007");
}

TEST_CASE("COPY8 - all time fields scroll by their component", "[events-screen]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();

    seq->init(7); // 8 bars.
    stateManager->drainQueue();
    seq->setTimeSignature(0, seq->getLastBarIndex(), 2, 4);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);
    mpc.getScreen()->function(1); // MAIN SCREEN > EDIT

    REQUIRE(layeredScreen->getCurrentScreenId() == ScreenId::EventsScreen);

    auto controls = mpc.getScreen();
    auto text = [&](const std::string &fieldName)
    {
        return controls->findChild<Field>(fieldName)->getText();
    };

    REQUIRE(text("time0") == "001");
    REQUIRE(text("time1") == "01");
    REQUIRE(text("time2") == "00");
    REQUIRE(text("time3") == "009");
    REQUIRE(text("time4") == "01");
    REQUIRE(text("time5") == "00");

    REQUIRE(layeredScreen->setFocus("time1"));
    controls->turnWheel(1);
    REQUIRE(text("time1") == "02");
    controls->turnWheel(1); // Clamp at max beat in 2/4.
    REQUIRE(text("time1") == "02");

    REQUIRE(layeredScreen->setFocus("time2"));
    controls->turnWheel(1);
    REQUIRE(text("time2") == "01");
    controls->turnWheel(-1);
    REQUIRE(text("time2") == "00");

    REQUIRE(layeredScreen->setFocus("time3"));
    controls->turnWheel(-1);
    REQUIRE(text("time3") == "008");

    REQUIRE(layeredScreen->setFocus("time4"));
    controls->turnWheel(1);
    REQUIRE(text("time4") == "02");
    controls->turnWheel(1); // Clamp at max beat in 2/4.
    REQUIRE(text("time4") == "02");

    REQUIRE(layeredScreen->setFocus("time5"));
    controls->turnWheel(1);
    REQUIRE(text("time5") == "01");
}

TEST_CASE("COPY9 - replace mode handles empty destination track",
          "[events-screen]")
{
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

    for (int i = 0; i < 4; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = i * 24;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
    }

    stateManager->drainQueue();

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    eventsScreen->performCopy(0, 96, SequenceIndex(0), 192, 1, false, 1, 34,
                              0);

    stateManager->drainQueue();

    const auto destTrack = seq->getTrack(1);
    REQUIRE(destTrack->getEvents().size() == 4);

    for (int i = 0; i < 4; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(destTrack->getEvent(i));
        REQUIRE(noteEvent->getTick() == 192 + i * 24);
        REQUIRE(noteEvent->getNote() == 35 + i);
    }
}

TEST_CASE("COPY10 - replace mode clears the actual destination range",
          "[events-screen]")
{
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

    for (int i = 0; i < 4; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = 96 + i * 24;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
    }

    for (int i = 0; i < 4; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = 240 + i * 24;
        eventData.noteNumber = NoteNumber(70 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
    }

    stateManager->drainQueue();

    auto eventsScreen = mpc.screens->get<ScreenId::EventsScreen>();

    eventsScreen->performCopy(96, 192, SequenceIndex(0), 240, 0, false, 1, 34,
                              0);

    stateManager->drainQueue();

    const auto events = tr->getEvents();
    REQUIRE(events.size() == 8);

    for (int i = 0; i < 4; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(events[i]);
        REQUIRE(noteEvent->getTick() == 96 + i * 24);
        REQUIRE(noteEvent->getNote() == 35 + i);
    }

    for (int i = 0; i < 4; i++)
    {
        auto noteEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(events[4 + i]);
        REQUIRE(noteEvent->getTick() == 240 + i * 24);
        REQUIRE(noteEvent->getNote() == 35 + i);
    }
}
