#include <catch2/catch_test_macros.hpp>

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/Track.hpp"
#include "TestMpc.hpp"

using namespace mpc::lcdgui::screens::window;

TEST_CASE("TimingCorrectScreen", "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    mpc.getSequencer()->getActiveSequence()->init(1);
    auto event = mpc.getSequencer()
                     ->getActiveSequence()
                     ->getTrack(0)
                     ->recordNoteEventSynced(1, 60, 127);
    mpc.getSequencer()
        ->getActiveSequence()
        ->getTrack(0)
        ->finalizeNoteEventSynced(event, 1);
    mpc.getLayeredScreen()->openScreen<TimingCorrectScreen>();
    auto controls = mpc.getScreen();
    controls->function(4); // DO IT
    REQUIRE(event->getTick() == 0);

    event->setTick(1);

    mpc.getLayeredScreen()->openScreen<TimingCorrectScreen>();
    controls->down();
    controls->down();
    controls->down();       // Move to 'Notes:' field.
    controls->turnWheel(1); // 'Notes:' was 'ALL', here we set it to '35/A01'.
    controls->function(4);  // DO IT
    REQUIRE(event->getTick() == 1);
}