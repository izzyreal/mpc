#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/SequenceStateView.hpp"

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("timing-correct", "[track]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    auto seq = sequencer->getSequence(0);
    seq->init(0);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    auto note88 = tr->recordNoteEventNonLive(12, NoteNumber(88), Velocity(127));
    tr->finalizeNoteEventNonLive(note88, Duration(1));

    auto note89 = tr->recordNoteEventNonLive(13, NoteNumber(89), Velocity(127));
    tr->finalizeNoteEventNonLive(note89, Duration(1));

    auto note90 = tr->recordNoteEventNonLive(23, NoteNumber(90), Velocity(127));
    tr->finalizeNoteEventNonLive(note90, Duration(1));

    auto note91 = tr->recordNoteEventNonLive(22, NoteNumber(91), Velocity(127));
    tr->finalizeNoteEventNonLive(note91, Duration(1));

    stateManager->drainQueue();

    REQUIRE(tr->getNoteEvents()[0]->getNote() == note88->noteNumber);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == note89->noteNumber);
    REQUIRE(tr->getNoteEvents()[2]->getNote() == note91->noteNumber);
    REQUIRE(tr->getNoteEvents()[3]->getNote() == note90->noteNumber);

    int swingPercentage = 50;

    tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0, note88,
                      note88->tick, 24, swingPercentage);
    stateManager->drainQueue();

    tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0, note89,
                      note89->tick, 24, swingPercentage);
    stateManager->drainQueue();

    tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0, note91,
                      note91->tick, 24, swingPercentage);
    stateManager->drainQueue();

    REQUIRE(tr->getNoteEvents()[0]->getNote().get() == 88);
    REQUIRE(tr->getNoteEvents()[0]->getTick() == 0);

    REQUIRE(tr->getNoteEvents()[2]->getNote().get() == 89);
    REQUIRE(tr->getNoteEvents()[2]->getTick() == 24);

    REQUIRE(tr->getNoteEvents()[1]->getNote().get() == 90);
    REQUIRE(tr->getNoteEvents()[1]->getTick() == 23);

    REQUIRE(tr->getNoteEvents()[3]->getNote().get() == 91);
    REQUIRE(tr->getNoteEvents()[3]->getTick() == 24);
}

TEST_CASE("swing1", "[track]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(0);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    auto n1 = tr->recordNoteEventNonLive(23, NoteNumber(90), Velocity(127));
    stateManager->drainQueue();
    tr->finalizeNoteEventNonLive(n1, Duration(1));

    auto n2 = tr->recordNoteEventNonLive(22, NoteNumber(91), Velocity(127));
    stateManager->drainQueue();
    tr->finalizeNoteEventNonLive(n2, Duration(1));

    stateManager->drainQueue();

    REQUIRE(tr->getEvent(0)->snapshot.noteNumber == n2->noteNumber);

    tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0, n2,
                      n2->tick, 24, 71);
    stateManager->drainQueue();

    REQUIRE(tr->getEvent(0)->getTick() == 23);
    REQUIRE(tr->getNoteEvents()[0]->getNote() == n1->noteNumber);

    REQUIRE(tr->getNoteEvents()[1]->getTick() == 34);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == n2->noteNumber);

    tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0, n2,
                      tr->getNoteEvents()[1]->getTick(), 24, 60);
    stateManager->drainQueue();

    REQUIRE(tr->getNoteEvents()[1]->getNote() == n2->noteNumber);
    REQUIRE(tr->getEvent(1)->getTick() == 28);
}

TEST_CASE("quantize", "[track]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(0);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    auto n0 = tr->recordNoteEventNonLive(0, NoteNumber(60), Velocity(127));
    tr->finalizeNoteEventNonLive(n0, Duration(1));

    stateManager->drainQueue();

    for (int i = 0; i <= 12; i++)
    {
        tr->getEvent(0)->setTick(i);
        stateManager->drainQueue();
        tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0,
                          tr->getEvent(0)->handle, i, 24, 50);
        stateManager->drainQueue();
        REQUIRE(tr->getEvent(0)->getTick() == 0);
    }

    for (int i = 13; i <= 36; i++)
    {
        tr->getEvent(0)->setTick(i);
        stateManager->drainQueue();
        tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0,
                          tr->getEvent(0)->handle, i, 24, 50);
        stateManager->drainQueue();
        REQUIRE(tr->getEvent(0)->getTick() == 24);
    }

    for (int i = 37; i <= 60; i++)
    {
        tr->getEvent(0)->setTick(i);
        stateManager->drainQueue();
        tr->timingCorrect(seq->getSnapshot(seq->getSequenceIndex()), 0, 0,
                          tr->getEvent(0)->handle, i, 24, 50);
        stateManager->drainQueue();
        REQUIRE(tr->getEvent(0)->getTick() == 48);
    }
}
