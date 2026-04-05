#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "controller/ClientEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "input/keyboard/KeyboardBindingsReader.hpp"
#include "input/keyboard/VmpcKeyCode.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "lcdgui/screens/window/VmpcResetKeyboardScreen.hpp"
#include "nvram/NvRam.hpp"

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::input::keyboard;

TEST_CASE("VmpcSettings persists Big time shift", "[vmpc-settings]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto settings = mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    REQUIRE_FALSE(settings->isBigTimeShiftEnabled());

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    auto controls = mpc.getScreen();
    controls->down();
    controls->down();
    controls->down();
    controls->down();
    controls->turnWheel(1);
    REQUIRE(settings->isBigTimeShiftEnabled());

    nvram::NvRam::saveVmpcSettings(mpc);

    controls->turnWheel(-1);
    REQUIRE_FALSE(settings->isBigTimeShiftEnabled());

    nvram::NvRam::loadVmpcSettings(mpc);
    REQUIRE(settings->isBigTimeShiftEnabled());
}

TEST_CASE("VmpcKeyboard reset stays dirty until saved", "[vmpc-keyboard]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto liveBindings = mpc.clientEventController->getKeyboardBindings();
    liveBindings->lookupFirstKeyCodeBinding(VmpcKeyCode::VMPC_KEY_LeftArrow)
        ->keyCode = VmpcKeyCode::VMPC_KEY_ANSI_Z;
    REQUIRE(liveBindings
                ->lookupComponentKeyCodes(
                    hardware::ComponentId::CURSOR_LEFT_OR_DIGIT)
                .front() == VmpcKeyCode::VMPC_KEY_ANSI_Z);

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcKeyboardScreen);
    auto keyboardScreen = mpc.screens->get<ScreenId::VmpcKeyboardScreen>();
    REQUIRE_FALSE(keyboardScreen->hasMappingChanged());

    keyboardScreen->function(4);
    auto resetScreen = mpc.screens->get<ScreenId::VmpcResetKeyboardScreen>();
    resetScreen->function(4);

    REQUIRE(keyboardScreen->hasMappingChanged());
    REQUIRE(liveBindings
                ->lookupComponentKeyCodes(
                    hardware::ComponentId::CURSOR_LEFT_OR_DIGIT)
                .front() == VmpcKeyCode::VMPC_KEY_ANSI_Z);

    keyboardScreen->function(5);

    REQUIRE_FALSE(keyboardScreen->hasMappingChanged());
    REQUIRE(liveBindings
                ->lookupComponentKeyCodes(
                    hardware::ComponentId::CURSOR_LEFT_OR_DIGIT)
                .front() == VmpcKeyCode::VMPC_KEY_LeftArrow);

    const auto persisted =
        KeyboardBindingsReader::fromJsonFile(mpc.paths->keyboardBindingsPath());
    REQUIRE(persisted.has_value());
    REQUIRE(KeyboardBindings(*persisted)
                .lookupComponentKeyCodes(
                    hardware::ComponentId::CURSOR_LEFT_OR_DIGIT)
                .front() == VmpcKeyCode::VMPC_KEY_LeftArrow);
}
