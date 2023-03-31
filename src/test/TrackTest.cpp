#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("timing-correct", "[track]")
{
    mpc::Mpc mpc;
    mpc.init(1, 5);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n0 = std::dynamic_pointer_cast<NoteOnEvent>(tr->addEvent(12, "note"));
    n0->setNote(88);

    auto n0a = std::dynamic_pointer_cast<NoteOnEvent>(tr->addEvent(13, "note"));
    n0a->setNote(89);

    auto n1 = std::dynamic_pointer_cast<NoteOnEvent>(tr->addEvent(23, "note"));
    n1->setNote(90);

    auto event2 = tr->addEvent(22, "note");

    REQUIRE(tr->getEvent(0) == n0);
    REQUIRE(tr->getEvent(1) == n0a);
    REQUIRE(tr->getEvent(2) == event2);
    REQUIRE(tr->getEvent(3) == n1);

    auto n2 = std::dynamic_pointer_cast<NoteOnEvent>(event2);
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
    mpc.init(1, 5);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n1 = std::dynamic_pointer_cast<NoteOnEvent>(tr->addEvent(23, "note"));
    n1->setNote(90);

    auto event2 = tr->addEvent(22, "note");

    REQUIRE(tr->getEvent(0) == event2);

    auto n2 = std::dynamic_pointer_cast<NoteOnEvent>(event2);
    n2->setNote(91);
    auto range = std::vector<int>{91, 91};
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
    mpc.init(1, 5);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n0 = std::dynamic_pointer_cast<NoteOnEvent>(tr->addEvent(0, "note"));

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