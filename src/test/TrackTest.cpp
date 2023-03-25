#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("swing1", "[track]")
{
    mpc::Mpc mpc;
    mpc.init(1, 5);
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(0);
    auto tr = seq->getTrack(0);

    auto n1 = std::dynamic_pointer_cast<NoteEvent>(tr->addEvent(23, "note"));
    n1->setNote(90);

    auto event2 = tr->addEvent(22, "note");

    REQUIRE(tr->getEvent(0) == event2);

    auto n2 = std::dynamic_pointer_cast<NoteEvent>(event2);
    n2->setNote(91);
    auto events = std::vector<std::shared_ptr<Event>>{event2};
    auto range = std::vector<int>{91, 91};
    tr->swing(events, 3, 71, range);

    REQUIRE(n1->getTick() == 23);
    REQUIRE(n1->getNote() == 90);
    REQUIRE(tr->getEvent(0) == n1);

    REQUIRE(n2->getTick() == 34);
    REQUIRE(n2->getNote() == 91);
    REQUIRE(tr->getEvent(1) == n2);
}