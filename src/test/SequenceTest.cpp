#include "sequencer/Sequence.hpp"
#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/NonRtSequencerStateManager.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("Insert bar and move event forward", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getNonRtStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    EventState eventState;
    eventState.type = EventType::NoteOn;
    eventState.tick = 0;
    eventState.noteNumber = mpc::MinDrumNoteNumber;
    eventState.velocity = mpc::MaxVelocity;
    eventState.duration = mpc::Duration(42);

    tr->insertEvent(eventState);
    stateManager->drainQueue();

    seq->insertBars(1, 0);
    stateManager->drainQueue();
    REQUIRE(tr->getEvent(0)->getTick() == 384);
}
