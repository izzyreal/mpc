#include "sequencer/Sequence.hpp"
#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/SequenceStateManager.hpp"
#include "sequencer/TrackEventStateManager.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("Insert bar and move event forward", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto seq = mpc.getSequencer()->getSelectedSequence();
    seq->init(0);
    seq->getStateManager()->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    EventData eventState;
    eventState.type = EventType::NoteOn;
    eventState.tick = 0;
    eventState.noteNumber = mpc::MinDrumNoteNumber;
    eventState.velocity = mpc::MaxVelocity;
    eventState.duration = mpc::Duration(42);

    tr->insertEvent(eventState);
    tr->getEventStateManager()->drainQueue();

    seq->insertBars(1, 0);
    seq->getStateManager()->drainQueue();
    tr->getEventStateManager()->drainQueue();
    REQUIRE(tr->getEvent(0)->getTick() == 384);
}
