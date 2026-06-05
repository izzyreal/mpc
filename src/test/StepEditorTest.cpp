#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/window/PasteEventScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

TEST_CASE("Step editor pastes the copied note after deleting the source event",
          "[step-editor]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto sequence = sequencer->getSelectedSequence();
    sequence->init(0);
    stateManager->drainQueue();

    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = NoteNumber(36);
    eventData.velocity = Velocity(60);
    eventData.duration = Duration(153);

    const auto track = sequence->getTrack(0);
    track->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    auto expectedEvent = track->getEventStates().at(0);

    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::StepEditorScreen);

    const auto stepEditorScreen = mpc.screens->get<ScreenId::StepEditorScreen>();
    REQUIRE(layeredScreen->setFocus("a0"));

    stepEditorScreen->function(1); // COPY
    REQUIRE(stepEditorScreen->getPlaceHolder().size() == 1);

    stepEditorScreen->function(2); // DELETE
    stateManager->drainQueue();
    layeredScreen->timerCallback();
    REQUIRE(track->getEventStates().empty());

    sequencer->goToNextStep();
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    stepEditorScreen->function(4); // PASTE

    const auto pasteEventScreen = mpc.screens->get<ScreenId::PasteEventScreen>();
    pasteEventScreen->function(4); // DO IT
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    expectedEvent.tick = 24;

    const auto eventStates = track->getEventStates();
    REQUIRE(eventStates.size() == 1);
    REQUIRE(eventStates[0] == expectedEvent);
}
