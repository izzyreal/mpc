#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequenceStateManager.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/TrackEventStateManager.hpp"

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("timing-correct", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    seq->getStateManager()->drainQueue();
    auto tr = seq->getTrack(0);

    auto note88 = tr->recordNoteEventNonLive(12, NoteNumber(88), Velocity(127));
    tr->finalizeNoteEventNonLive(note88, Duration(1));

    auto note89 = tr->recordNoteEventNonLive(13, NoteNumber(89), Velocity(127));
    tr->finalizeNoteEventNonLive(note89, Duration(1));

    auto note90 = tr->recordNoteEventNonLive(23, NoteNumber(90), Velocity(127));
    tr->finalizeNoteEventNonLive(note90, Duration(1));

    auto note91 = tr->recordNoteEventNonLive(22, NoteNumber(91), Velocity(127));
    tr->finalizeNoteEventNonLive(note91, Duration(1));

    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getNoteEvents()[0]->getNote() == note88.noteNumber);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == note89.noteNumber);
    REQUIRE(tr->getNoteEvents()[2]->getNote() == note91.noteNumber);
    REQUIRE(tr->getNoteEvents()[3]->getNote() == note90.noteNumber);

    int swingPercentage = 50;
    tr->timingCorrect(0, 0, tr->getNoteEvents()[0]->getSnapshot(), 24, swingPercentage);
    tr->getEventStateManager()->drainQueue();
    REQUIRE(tr->getNoteEvents()[0]->getNote() == 88);
    REQUIRE(tr->getNoteEvents()[0]->getTick() == 0);

    tr->timingCorrect(0, 0, tr->getNoteEvents()[1]->getSnapshot(), 24, swingPercentage);
    tr->getEventStateManager()->drainQueue();
    REQUIRE(tr->getNoteEvents()[3]->getNote() == 89);
    REQUIRE(tr->getNoteEvents()[3]->getTick() == 24);

    REQUIRE(tr->getNoteEvents()[1]->getNote() == 91);
    REQUIRE(tr->getNoteEvents()[1]->getTick() == 22);

    tr->timingCorrect(0, 0, tr->getNoteEvents()[1]->getSnapshot(), 24, swingPercentage);
    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getNoteEvents()[0]->getNote() == 88);
    REQUIRE(tr->getNoteEvents()[0]->getTick() == 0);

    REQUIRE(tr->getNoteEvents()[3]->getNote() == 89);
    REQUIRE(tr->getNoteEvents()[3]->getTick() == 24);

    REQUIRE(tr->getNoteEvents()[1]->getNote() == 90);
    REQUIRE(tr->getNoteEvents()[1]->getTick() == 23);

    REQUIRE(tr->getNoteEvents()[2]->getNote() == 91);
    REQUIRE(tr->getNoteEvents()[2]->getTick() == 24);
}

TEST_CASE("swing1", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    seq->getStateManager()->drainQueue();
    mpc.getSequencer()->getStateManager()->drainQueue();
    auto tr = seq->getTrack(0);

    auto n1 = tr->recordNoteEventNonLive(23, NoteNumber(90), Velocity(127));
    tr->getEventStateManager()->drainQueue();
    tr->finalizeNoteEventNonLive(n1, Duration(1));

    auto n2 = tr->recordNoteEventNonLive(22, NoteNumber(91), Velocity(127));
    tr->getEventStateManager()->drainQueue();
    tr->finalizeNoteEventNonLive(n2, Duration(1));

    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvent(0)->getSnapshot().second.noteNumber == n2.noteNumber);

    tr->timingCorrect(0, 0, tr->getEvent(0)->getSnapshot(), 24, 71);
    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvent(0)->getTick() == 23);
    REQUIRE(tr->getNoteEvents()[0]->getNote() == 90);

    REQUIRE(tr->getNoteEvents()[1]->getTick() == 34);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == 91);

    tr->timingCorrect(0, 0, tr->getEvent(1)->getSnapshot(), 24, 60);
    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvent(1)->getTick() == 28);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == 91);
}

TEST_CASE("quantize", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    seq->getStateManager()->drainQueue();
    auto tr = seq->getTrack(0);

    auto n0 = tr->recordNoteEventNonLive(0, NoteNumber(60), Velocity(127));
    tr->finalizeNoteEventNonLive(n0, Duration(1));

    tr->getEventStateManager()->drainQueue();

    for (int i = 0; i <= 12; i++)
    {
        tr->getEvent(0)->setTick(i);
        tr->getEventStateManager()->drainQueue();
        tr->timingCorrect(0, 0, tr->getEvent(0)->getSnapshot(), 24, 50);
        tr->getEventStateManager()->drainQueue();
        REQUIRE(tr->getEvent(0)->getTick() == 0);
    }

    for (int i = 13; i <= 36; i++)
    {
        tr->getEvent(0)->setTick(i);
        tr->getEventStateManager()->drainQueue();
        tr->timingCorrect(0, 0, tr->getEvent(0)->getSnapshot(), 24, 50);
        tr->getEventStateManager()->drainQueue();
        REQUIRE(tr->getEvent(0)->getTick() == 24);
    }

    for (int i = 37; i <= 60; i++)
    {
        tr->getEvent(0)->setTick(i);
        tr->getEventStateManager()->drainQueue();
        tr->timingCorrect(0, 0, tr->getEvent(0)->getSnapshot(), 24, 50);
        tr->getEventStateManager()->drainQueue();
        REQUIRE(tr->getEvent(0)->getTick() == 48);
    }
}
