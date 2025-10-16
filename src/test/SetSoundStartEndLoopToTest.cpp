#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "TestMpc.hpp"
#include "hardware/Hardware.h"
#include "sampler/Sampler.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
#include "lcdgui/screens/LoopScreen.hpp"
#include "lcdgui/screens/window/StartFineScreen.hpp"
#include "lcdgui/screens/window/EndFineScreen.hpp"
#include "lcdgui/screens/window/LoopToFineScreen.hpp"

using namespace mpc::sampler;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

const uint16_t SOUND_LENGTH = 1000;
const uint16_t MAX_SLIDER_VALUE = 127;

std::shared_ptr<Sound> initSound(mpc::Mpc& mpc)
{
    auto sound = mpc.getSampler()->addSound("");
    assert(sound != nullptr);
    sound->setMono(true);
    std::vector<float> silence(SOUND_LENGTH);
    sound->insertFrames(silence, 0, SOUND_LENGTH);
    sound->setEnd(SOUND_LENGTH);
    sound->setLoopTo(SOUND_LENGTH);

    assert(sound->getStart() == 0);
    assert(sound->getEnd() == SOUND_LENGTH);

    return sound;
}

TEST_CASE("TRIM screen", "[sound]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sound = initSound(mpc);

    mpc.getLayeredScreen()->openScreen("trim");

    auto trimScreen = mpc.screens->get<TrimScreen>("trim");

    trimScreen->down(); // Move to St: field
    trimScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    trimScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    trimScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);

    trimScreen->right(); // Move to End: field
    trimScreen->turnWheel(1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);
    trimScreen->turnWheel(-1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH - 1);

    sound->setStart(250);
    sound->setEnd(500);

    trimScreen->left();
    trimScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 249);
    REQUIRE(sound->getEnd() == 500);

    trimScreen->right();
    trimScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 249);
    REQUIRE(sound->getEnd() == 499);

    auto shiftButton = mpc.getHardware()->getButton("shift");
    shiftButton->press();
    trimScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getStart() == 249);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();
    trimScreen->left(); // Back to St: field

    shiftButton->press();
    trimScreen->setSlider(0);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    trimScreen->setSlider(MAX_SLIDER_VALUE);

    REQUIRE(sound->getStart() == SOUND_LENGTH);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();

    trimScreen->openWindow();

    auto startFineScreen = mpc.screens->get<StartFineScreen>("start-fine");

    startFineScreen->down(); // Move to Smpl length: field
    startFineScreen->turnWheel(1); // Set Smpl length from VARI to FIX

    mpc.getLayeredScreen()->openScreen("trim");

    sound->setStart(250);
    sound->setEnd(500);

    // We're now in fixed length mode, so start and end are expected to move in tandem
    trimScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 251);
    REQUIRE(sound->getEnd() == 501);

    trimScreen->right();
    trimScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getEnd() == 500);

    // ... but within limits
    trimScreen->turnWheel(-250);

    // So this should be a no-op
    trimScreen->turnWheel(-1);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 250);

    trimScreen->left(); // Move to St: field
    trimScreen->turnWheel(750);

    // And this should be a no-op too
    trimScreen->turnWheel(1);

    REQUIRE(sound->getStart() == 750);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->press();
    trimScreen->setSlider(0);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 250);
    trimScreen->setSlider(MAX_SLIDER_VALUE);

    REQUIRE(sound->getStart() == 750);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();
    trimScreen->right(); // Move to End: field

    shiftButton->press();
    trimScreen->setSlider(0);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 250);
    trimScreen->setSlider(MAX_SLIDER_VALUE);

    REQUIRE(sound->getStart() == 750);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);
}

TEST_CASE("Start fine window", "[sound]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sound = initSound(mpc);

    mpc.getLayeredScreen()->openScreen("trim");
    auto trimScreen = mpc.screens->get<TrimScreen>("trim");
    trimScreen->down(); // Move to St: field
    trimScreen->openWindow(); // Open Start fine window
    auto startFineScreen = mpc.screens->get<StartFineScreen>("start-fine");
    // Try to decrease Start by 1, but it's expected to be at 0 so shouldn't go any lower
    startFineScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    startFineScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    auto shiftButton = mpc.getHardware()->getButton("shift");

    shiftButton->press();
    startFineScreen->setSlider(0);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    startFineScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getStart() == SOUND_LENGTH);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();

    sound->setStart(250);
    sound->setEnd(500);

    startFineScreen->down(); // Move to Smpl length: field
    startFineScreen->turnWheel(1); // Set Smpl length from VARI to FIX

    startFineScreen->up();
    startFineScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 249);
    REQUIRE(sound->getEnd() == 499);

    shiftButton->press();

    startFineScreen->setSlider(0);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 250);

    startFineScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getStart() == 750);
    REQUIRE(sound->getEnd() == 1000);
}

TEST_CASE("End fine window", "[sound]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sound = initSound(mpc);

    mpc.getLayeredScreen()->openScreen("trim");
    auto trimScreen = mpc.screens->get<TrimScreen>("trim");
    trimScreen->down(); // Move to St: field
    trimScreen->right(); // Move to End: field
    trimScreen->openWindow(); // Open End fine window
    auto endFineScreen = mpc.screens->get<EndFineScreen>("end-fine");
    // Try to increase End by 1, but it's expected to be at SOUND_LENGTH so shouldn't go any higher
    endFineScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    endFineScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH - 1);

    auto shiftButton = mpc.getHardware()->getButton("shift");
    shiftButton->press();

    endFineScreen->setSlider(0);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 0);

    endFineScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();

    sound->setStart(250);
    sound->setEnd(500);

    endFineScreen->down(); // Move to Smpl length: field
    endFineScreen->turnWheel(1); // Set Smpl length from VARI to FIX

    endFineScreen->up();
    endFineScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 249);
    REQUIRE(sound->getEnd() == 499);

    shiftButton->press();
    endFineScreen->setSlider(0);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 250);

    endFineScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getStart() == 750);
    REQUIRE(sound->getEnd() == 1000);
}

TEST_CASE("LOOP screen with end", "[sound]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sound = initSound(mpc);

    mpc.getLayeredScreen()->openScreen("loop");

    auto loopScreen = mpc.screens->get<LoopScreen>("loop");

    loopScreen->down(); // Move to To: field
    loopScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH - 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    sound->setEnd(SOUND_LENGTH - 1);

    loopScreen->turnWheel(1);
    // Loop To may not change End value, contrary to how St and End may affect each other
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH - 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH -1);

    sound->setEnd(SOUND_LENGTH);

    loopScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    loopScreen->right(); // Not a typo, this screen has a field between To: and End:
    loopScreen->right(); // Move to End: field
    loopScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    loopScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    // End value may lower the Loop To value
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH - 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH - 1);

    sound->setLoopTo(250);
    sound->setEnd(500);

    loopScreen->left();loopScreen->left(); // Move back to To: field
    loopScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 249);
    REQUIRE(sound->getEnd() == 500);

    loopScreen->right();loopScreen->right(); // Move back to End: field
    loopScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 249);
    REQUIRE(sound->getEnd() == 499);

    auto shiftButton = mpc.getHardware()->getButton("shift");

    shiftButton->press();
    loopScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 249);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();
    loopScreen->left();loopScreen->left(); // Back to To: field

    shiftButton->press();
    loopScreen->setSlider(0);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 0);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    loopScreen->setSlider(MAX_SLIDER_VALUE);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    sound->setLoopTo(250);
    sound->setEnd(500);

    loopScreen->setSlider(MAX_SLIDER_VALUE);
    REQUIRE(sound->getLoopTo() == 500);
    REQUIRE(sound->getEnd() == 500);

    shiftButton->release();

    loopScreen->openWindow(); // Open Loop To fine screen

    auto loopToFineScreen = mpc.screens->get<LoopToFineScreen>("loop-to-fine");

    loopToFineScreen->down();loopToFineScreen->down(); // Move to Loop Lngth: field
    loopToFineScreen->turnWheel(1); // Set Loop Lngth from VARI to FIX

    mpc.getLayeredScreen()->openScreen("loop");

    sound->setStart(0);
    sound->setLoopTo(250);
    sound->setEnd(500);

    // We're now in fixed loop length mode, so loop to and end
    // are expected to move in tandem while leaving start untouched
    loopScreen->turnWheel(1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 251);
    REQUIRE(sound->getEnd() == 501);

    loopScreen->right();loopScreen->right(); // Move to End: field
    loopScreen->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 250);
    REQUIRE(sound->getEnd() == 500);

    // ... but within limits
    loopScreen->turnWheel(-250);

    // So this should be a no-op
    loopScreen->turnWheel(-1);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 0);
    REQUIRE(sound->getEnd() == 250);

    loopScreen->left();loopScreen->left(); // Move to To: field
    loopScreen->turnWheel(750);

    // And this should be a no-op too
    loopScreen->turnWheel(1);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 750);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->press();
    loopScreen->setSlider(0);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 0);
    REQUIRE(sound->getEnd() == 250);

    loopScreen->setSlider(MAX_SLIDER_VALUE);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 750);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    shiftButton->release();
    loopScreen->right();loopScreen->right(); // Move to End: field

    shiftButton->press();
    loopScreen->setSlider(0);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 0);
    REQUIRE(sound->getEnd() == 250);
    loopScreen->setSlider(MAX_SLIDER_VALUE);

    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == 750);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);
}

// In the LOOP screen, the End: field can be changed to be a Lngth: field.
TEST_CASE("LOOP screen with loop length manipulation", "[sound]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sound = initSound(mpc);

    mpc.getLayeredScreen()->openScreen("loop");

    auto loopScreen = mpc.screens->get<LoopScreen>("loop");

    loopScreen->down();
    loopScreen->right(); // Move to Lngth: field (label)
    loopScreen->turnWheel(-1); // Set End: field to Lngth: field
    loopScreen->right(); // Move to Lngth: field (value)
    sound->setStart(250);
    sound->setEnd(750);
    sound->setLoopTo(500);

    loopScreen->turnWheel(1);
    // We have added 1 to the length. When sound length is VARiable,
    // which is the default and is active now, changing the length is
    // done by changing the end. When sound length is FIXed, changing
    // the length is done by changing the loop-to point.
    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getLoopTo() == 500);
    REQUIRE(sound->getEnd() == 751);

    loopScreen->function(0); // Go to TRIM screen
    mpc.getActiveControls()->down();
    mpc.getActiveControls()->openWindow(); // Open Start fine window
    mpc.getActiveControls()->down(); // Move to Smpl Length: field
    mpc.getActiveControls()->turnWheel(1); // Set Smpl Length to FIXed
    mpc.getActiveControls()->function(3); // Close Start fine window
    mpc.getActiveControls()->function(1); // Open LOOP screen, and we should be back at the Lngth: 's value field
    mpc.getActiveControls()->turnWheel(1); // Add 1 to the length

    // Now we expect the loop-to point to have changed. When increasing the loop length,
    // the loop-to point becomes lower.
    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getLoopTo() == 499);
    REQUIRE(sound->getEnd() == 751);

    mpc.getActiveControls()->openWindow(); // Open Loop End fine window
    mpc.getActiveControls()->down();
    mpc.getActiveControls()->down(); // Move to Loop Lngth: field
    mpc.getActiveControls()->turnWheel(1); // Set Loop Lngth to FIXed
    mpc.getActiveControls()->function(3); // Go back to LOOP screen, back at Lgnth: 's value field
    mpc.getActiveControls()->turnWheel(1); // Loop Lngth is FIXed, so we expect this to be a no-op

    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getLoopTo() == 499);
    REQUIRE(sound->getEnd() == 751);
}

TEST_CASE("Loop To fine window", "[sound]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sound = initSound(mpc);

    mpc.getLayeredScreen()->openScreen("loop");

    mpc.getActiveControls()->down(); // Move to To: field
    mpc.getActiveControls()->openWindow(); // Open Loop To fine window

    mpc.getActiveControls()->turnWheel(-1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH - 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    sound->setEnd(SOUND_LENGTH - 1);

    mpc.getActiveControls()->turnWheel(1);
    // Loop To may not change End value, contrary to how St and End may affect each other
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH - 1);
    REQUIRE(sound->getEnd() == SOUND_LENGTH - 1);

    sound->setEnd(SOUND_LENGTH);

    mpc.getActiveControls()->turnWheel(1);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getLoopTo() == SOUND_LENGTH);
    REQUIRE(sound->getEnd() == SOUND_LENGTH);

    // Loop To fine's To: field behaves like the LOOP screen To: field. Great!

    // Now we'll check if Loop To fine's Lngth: field behaves like the LOOP screen Lngth:

    sound->setStart(250);
    sound->setEnd(750);
    sound->setLoopTo(500);

    mpc.getActiveControls()->down(); // Move to Lngth: field
    mpc.getActiveControls()->turnWheel(1);
    // We have added 1 to the length. When sound length is VARiable,
    // which is the default and is active now, changing the length is
    // done by changing the end. When sound length is FIXed, changing
    // the length is done by changing the loop-to point.
    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getLoopTo() == 500);
    REQUIRE(sound->getEnd() == 751);

    mpc.getActiveControls()->function(3); // Go back to LOOP screen
    mpc.getActiveControls()->function(0); // Go to TRIM screen
    mpc.getActiveControls()->down();
    mpc.getActiveControls()->openWindow(); // Open Start fine window
    mpc.getActiveControls()->down(); // Move to Smpl Length: field
    mpc.getActiveControls()->turnWheel(1); // Set Smpl Length to FIXed
    mpc.getActiveControls()->function(3); // Close Start fine window
    mpc.getActiveControls()->function(1); // Open LOOP screen, and we should be back at the To: field

    mpc.getActiveControls()->openWindow(); // Open Loop To fine window, we should be back at the Lngth: field
    mpc.getActiveControls()->turnWheel(1); // Add 1 to the length

    // Now we expect the loop-to point to have changed. When increasing the loop length,
    // the loop-to point becomes lower.
    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getLoopTo() == 499);
    REQUIRE(sound->getEnd() == 751);

    mpc.getActiveControls()->down(); // Move to Loop Lngth: field
    mpc.getActiveControls()->turnWheel(1); // Set Loop Lngth to FIXed
    mpc.getActiveControls()->up(); // Go back to LOOP Lngth: field
    mpc.getActiveControls()->turnWheel(1); // Loop Lngth is FIXed, so we expect this to be a no-op

    REQUIRE(sound->getStart() == 250);
    REQUIRE(sound->getLoopTo() == 499);
    REQUIRE(sound->getEnd() == 751);
}
