#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "TestMpc.hpp"
#include "engine/EngineHost.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

using namespace mpc;
using namespace mpc::input::midi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

TEST_CASE("Activating a MIDI control preset persists it for the next startup",
          "[midi-control][persistence]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    const auto presetPath =
        mpc.paths->getDocuments()->midiControlPresetsPath() / "MPD218.json";
    const auto presetResult =
        MidiControlPresetUtil::loadPresetFromFile(presetPath);
    REQUIRE(presetResult);

    const auto midiScreen = mpc.screens->get<ScreenId::VmpcMidiScreen>();
    REQUIRE(midiScreen->activatePreset(*presetResult));

    mpc.getEngineHost()->switchMidiControlMappingIfRequired();
    CHECK(midiScreen->getActivePreset()->getName() == "MPD218");

    const auto persistedResult = MidiControlPresetUtil::loadPresetFromFile(
        mpc.paths->getDocuments()->activeMidiControlPresetPath());
    REQUIRE(persistedResult);
    CHECK((*persistedResult)->getName() == "MPD218");
    CHECK((*persistedResult)->getBindings() ==
          (*presetResult)->getBindings());
}
