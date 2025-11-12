#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("timing-correct", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n0 = tr->recordNoteEventSynced(12, 88, 127);
    tr->finalizeNoteEventSynced(n0, 1);

    auto n0a = tr->recordNoteEventSynced(13, 89, 127);
    tr->finalizeNoteEventSynced(n0a, 1);

    auto n1 = tr->recordNoteEventSynced(23, 90, 127);
    tr->finalizeNoteEventSynced(n1, 1);

    auto event2 = tr->recordNoteEventSynced(22, 60, 127);
    tr->finalizeNoteEventSynced(event2, 1);

    REQUIRE(tr->getEvent(0) == n0);
    REQUIRE(tr->getEvent(1) == n0a);
    REQUIRE(tr->getEvent(2) == event2);
    REQUIRE(tr->getEvent(3) == n1);

    auto n2 = event2;
    n2->setNote(91);
    int swingPercentage = 50;
    tr->timingCorrect(0, 0, n0, 24, swingPercentage);
    tr->timingCorrect(0, 0, n0a, 24, swingPercentage);
    tr->timingCorrect(0, 0, n2, 24, swingPercentage);

    REQUIRE(n0->getTick() == 0);
    REQUIRE(n0->getNote() == 88);
    REQUIRE(tr->getEvent(0) == n0);

    REQUIRE(n0a->getTick() == 24);
    REQUIRE(n0a->getNote() == 89);
    REQUIRE(tr->getEvent(2) == n0a);

    REQUIRE(n1->getTick() == 23);
    REQUIRE(n1->getNote() == 90);
    REQUIRE(tr->getEvent(1) == n1);

    REQUIRE(n2->getTick() == 24);
    REQUIRE(n2->getNote() == 91);
    REQUIRE(tr->getEvent(3) == n2);
}

TEST_CASE("swing1", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n1 = tr->recordNoteEventSynced(23, 90, 127);
    tr->finalizeNoteEventSynced(n1, 1);

    auto event2 = tr->recordNoteEventSynced(22, 91, 127);
    tr->finalizeNoteEventSynced(event2, 1);

    REQUIRE(tr->getEvent(0) == event2);

    auto n2 = event2;
    auto range = std::vector{91, 91};
    tr->timingCorrect(0, 0, n2, 24, 71);

    REQUIRE(n1->getTick() == 23);
    REQUIRE(n1->getNote() == 90);
    REQUIRE(tr->getEvent(0) == n1);

    REQUIRE(n2->getTick() == 34);
    REQUIRE(n2->getNote() == 91);
    REQUIRE(tr->getEvent(1) == n2);

    tr->timingCorrect(0, 0, n2, 24, 60);

    REQUIRE(n2->getTick() == 28);
    REQUIRE(n2->getNote() == 91);
    REQUIRE(tr->getEvent(1) == n2);
}

TEST_CASE("quantize", "[track]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n0 = tr->recordNoteEventSynced(0, 60, 127);
    tr->finalizeNoteEventSynced(n0, 1);

    for (int i = 0; i <= 12; i++)
    {
        n0->setTick(i);
        tr->timingCorrect(0, 0, n0, 24, 50);
        REQUIRE(n0->getTick() == 0);
    }

    for (int i = 13; i <= 36; i++)
    {
        n0->setTick(i);
        tr->timingCorrect(0, 0, n0, 24, 50);
        REQUIRE(n0->getTick() == 24);
    }

    for (int i = 37; i <= 60; i++)
    {
        n0->setTick(i);
        tr->timingCorrect(0, 0, n0, 24, 50);
        REQUIRE(n0->getTick() == 48);
    }
}
