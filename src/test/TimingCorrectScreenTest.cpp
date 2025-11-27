#include <catch2/catch_test_macros.hpp>

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "TestMpc.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/NonRtSequencerStateManager.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

TEST_CASE("TimingCorrectScreen", "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getNonRtStateManager();

    sequencer->getSelectedSequence()->init(1);
    stateManager->drainQueue();

    mpc::sequencer::EventState eventState;
    eventState.type = mpc::sequencer::EventType::NoteOn;
    eventState.tick = 1;
    eventState.noteNumber = mpc::NoteNumber(60);
    eventState.velocity = mpc::MaxVelocity;
    eventState.duration = mpc::Duration(1);

    auto tr = sequencer->getSelectedSequence()->getTrack(0);

    tr->insertEvent(eventState);

    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    auto controls = mpc.getScreen();
    controls->function(4); // DO IT
    stateManager->drainQueue();
    REQUIRE(tr->getNoteEvents().front()->getTick() == 0);

    tr->getNoteEvents().front()->setTick(1);

    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    controls->down();
    controls->down();
    controls->down();       // Move to 'Notes:' field.
    controls->turnWheel(1); // 'Notes:' was 'ALL', here we set it to '35/A01'.
    controls->function(4);  // DO IT
    stateManager->drainQueue();
    REQUIRE(tr->getNoteEvents().front()->getTick() == 1);
}
