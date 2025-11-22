#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
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
    auto tr = seq->getTrack(0);

    auto n0 = tr->recordNoteEventNonLive(12, NoteNumber(88), Velocity(127));
    tr->finalizeNoteEventNonLive(n0, Duration(1));

    auto n0a = tr->recordNoteEventNonLive(13, NoteNumber(89), Velocity(127));
    tr->finalizeNoteEventNonLive(n0a, Duration(1));

    auto n1 = tr->recordNoteEventNonLive(23, NoteNumber(90), Velocity(127));
    tr->finalizeNoteEventNonLive(n1, Duration(1));

    auto event2 = tr->recordNoteEventNonLive(22, NoteNumber(60), Velocity(127));
    tr->finalizeNoteEventNonLive(event2, Duration(1));

    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getNoteEvents()[0]->getNote() == n0.noteNumber);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == n0a.noteNumber);
    REQUIRE(tr->getNoteEvents()[2]->getNote() == event2.noteNumber);
    REQUIRE(tr->getNoteEvents()[3]->getNote() == n1.noteNumber);

    tr->getNoteEvents()[2]->setNote(NoteNumber(91));
    tr->getEventStateManager()->drainQueue();

    int swingPercentage = 50;
    tr->timingCorrect(0, 0, tr->getNoteEvents()[0]->getSnapshot(), 24, swingPercentage);
    tr->timingCorrect(0, 0, tr->getNoteEvents()[1]->getSnapshot(), 24, swingPercentage);
    tr->timingCorrect(0, 0, tr->getNoteEvents()[2]->getSnapshot(), 24, swingPercentage);
    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getNoteEvents()[0]->getTick() == 0);
    REQUIRE(tr->getNoteEvents()[0]->getNote() == 88);

    REQUIRE(tr->getNoteEvents()[1]->getTick() == 24);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == 89);

    REQUIRE(tr->getNoteEvents()[2]->getTick() == 23);
    REQUIRE(tr->getNoteEvents()[2]->getNote() == 90);

    REQUIRE(tr->getNoteEvents()[3]->getTick() == 24);
    REQUIRE(tr->getNoteEvents()[3]->getNote() == 91);
}

TEST_CASE("swing1", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n1 = tr->recordNoteEventNonLive(23, NoteNumber(90), Velocity(127));
    tr->finalizeNoteEventNonLive(n1, Duration(1));

    auto event2 = tr->recordNoteEventNonLive(22, NoteNumber(91), Velocity(127));
    tr->finalizeNoteEventNonLive(event2, Duration(1));

    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvent(0)->getSnapshot().second == event2);

    auto n2 = event2;
    auto range = std::vector{91, 91};
    tr->timingCorrect(0, 0, tr->getEvent(1)->getSnapshot(), 24, 71);
    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvent(0)->getTick() == 23);
    REQUIRE(tr->getNoteEvents()[0]->getNote() == 90);
    REQUIRE(tr->getEvent(0)->getSnapshot().second == n1);

    REQUIRE(tr->getNoteEvents()[1]->getTick() == 34);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == 91);
    REQUIRE(tr->getEvent(1)->getSnapshot().second == n2);

    tr->timingCorrect(0, 0, tr->getEvent(1)->getSnapshot(), 24, 60);
    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvent(1)->getTick() == 28);
    REQUIRE(tr->getNoteEvents()[1]->getNote() == 91);
    REQUIRE(tr->getEvent(1)->getSnapshot().second == n2);
}

TEST_CASE("quantize", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
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
