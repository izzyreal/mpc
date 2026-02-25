#include <catch2/catch_test_macros.hpp>

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "TestMpc.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;

namespace
{
    void focusFieldByName(const std::shared_ptr<ScreenComponent> &screen,
                          const std::string &fieldName)
    {
        for (int i = 0; i < 16; ++i)
        {
            if (screen->getFocusedFieldNameOrThrow() == fieldName)
            {
                return;
            }
            screen->down();
        }
    }
}

TEST_CASE("TimingCorrectScreen", "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->getSelectedSequence()->init(1);
    stateManager->drainQueue();

    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 1;
    eventData.noteNumber = mpc::NoteNumber(60);
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(1);

    auto tr = sequencer->getSelectedSequence()->getTrack(0);

    tr->acquireAndInsertEvent(eventData);

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

TEST_CASE("TimingCorrectScreen amount clamp follows Big time shift setting",
          "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->getSelectedSequence()->init(1);
    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    auto timingScreen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
    auto controls = mpc.getScreen();

    controls->up();
    controls->up();
    controls->right();
    controls->down();
    REQUIRE(controls->getFocusedFieldNameOrThrow() == "amount");
    controls->turnWheel(500);
    REQUIRE(timingScreen->getAmount() == 11);

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    controls = mpc.getScreen();
    focusFieldByName(controls, "big-time-shift");
    controls->turnWheel(1);

    auto settingsScreen = mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    REQUIRE(settingsScreen->isBigTimeShiftEnabled());

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    timingScreen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
    controls = mpc.getScreen();

    controls->up();
    controls->up();
    controls->right();
    controls->down();
    REQUIRE(controls->getFocusedFieldNameOrThrow() == "amount");
    controls->turnWheel(500);
    REQUIRE(timingScreen->getAmount() == 99);

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    controls = mpc.getScreen();
    focusFieldByName(controls, "big-time-shift");
    controls->turnWheel(-1);

    REQUIRE_FALSE(settingsScreen->isBigTimeShiftEnabled());
    REQUIRE(timingScreen->getAmount() == 11);
}
