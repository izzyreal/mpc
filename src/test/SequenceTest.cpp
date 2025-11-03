#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("Insert bar and move event forward", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto seq = mpc.getSequencer()->getActiveSequence();
    seq->init(0);
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    auto noteEvent = std::make_shared<NoteOnEvent>(35);
    noteEvent->setVelocity(127);
    noteEvent->setDuration(42);
    noteEvent->setTick(0);
    tr->insertEventWhileRetainingSort(noteEvent);

    seq->insertBars(1, 0);
    REQUIRE(tr->getEvent(0)->getTick() == 384);
}
