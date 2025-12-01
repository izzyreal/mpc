#include "sequencer/Sequence.hpp"
#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("Insert bar and move event forward", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = mpc::MinDrumNoteNumber;
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(42);

    tr->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    seq->insertBars(1, mpc::BarIndex(0));
    stateManager->drainQueue();
    REQUIRE(tr->getEvent(0)->getTick() == 384);
}
