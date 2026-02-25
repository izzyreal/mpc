#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "nvram/NvRam.hpp"

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

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
