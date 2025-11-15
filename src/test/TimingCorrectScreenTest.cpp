#include <catch2/catch_test_macros.hpp>

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "TestMpc.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

TEST_CASE("TimingCorrectScreen", "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    mpc.getSequencer()->getSelectedSequence()->init(1);
    auto event = mpc.getSequencer()
                     ->getSelectedSequence()
                     ->getTrack(0)
                     ->recordNoteEventNonLive(1, mpc::NoteNumber(60), mpc::Velocity(127));
    event->finalizeNonLive(1);
    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    auto controls = mpc.getScreen();
    controls->function(4); // DO IT
    REQUIRE(event->getTick() == 0);

    event->setTick(1);

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    controls->down();
    controls->down();
    controls->down();       // Move to 'Notes:' field.
    controls->turnWheel(1); // 'Notes:' was 'ALL', here we set it to '35/A01'.
    controls->function(4);  // DO IT
    REQUIRE(event->getTick() == 1);
}
