#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "controller/ClientEventController.hpp"
#include "engine/EngineHost.hpp"
#include "engine/PhysicalInteractionSoundPlayer.hpp"
#include "hardware/ComponentId.hpp"
#include "input/keyboard/KeyboardBindingsReader.hpp"
#include "input/keyboard/VmpcKeyCode.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "lcdgui/screens/window/VmpcResetKeyboardScreen.hpp"
#include "nvram/NvRam.hpp"
#include "mpc_fs.hpp"

#include <array>

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

TEST_CASE("VmpcSettings persists physical sound controls",
          "[vmpc-settings][physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto settings = mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    REQUIRE_FALSE(settings->arePhysicalSoundsEnabled());
    REQUIRE(settings->getPhysicalSoundsMixMode() == 0);
    REQUIRE(settings->getPhysicalSoundsLevel() == 20);
    REQUIRE_FALSE(mpc.getEngineHost()->arePhysicalSoundsEnabled());
    REQUIRE(mpc.getEngineHost()->getPhysicalSoundsMixMode() ==
            engine::PhysicalSoundsMixMode::StereoOut);
    REQUIRE(mpc.getEngineHost()->getPhysicalSoundsLevel() == 20);
    for (const auto group :
         {engine::PhysicalSoundGroup::Buttons, engine::PhysicalSoundGroup::Pads,
          engine::PhysicalSoundGroup::Slider,
          engine::PhysicalSoundGroup::DataWheel,
          engine::PhysicalSoundGroup::Power})
    {
        REQUIRE(mpc.getEngineHost()->getPhysicalSoundGroupLevel(group) == 100);
    }

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    auto controls = mpc.getScreen();

    for (int i = 0; i < 5; ++i)
    {
        controls->down();
    }
    controls->turnWheel(1);
    controls->down();
    controls->turnWheel(1);
    controls->down();
    controls->turnWheel(62);

    const std::array groupLevels{89, 78, 67, 56, 45};
    const std::array groups{engine::PhysicalSoundGroup::Buttons,
                            engine::PhysicalSoundGroup::Pads,
                            engine::PhysicalSoundGroup::Slider,
                            engine::PhysicalSoundGroup::DataWheel,
                            engine::PhysicalSoundGroup::Power};
    for (size_t i = 0; i < groups.size(); ++i)
    {
        mpc.getEngineHost()->setPhysicalSoundGroupLevel(groups[i],
                                                        groupLevels[i]);
    }

    REQUIRE(settings->arePhysicalSoundsEnabled());
    REQUIRE(settings->getPhysicalSoundsMixMode() == 1);
    REQUIRE(settings->getPhysicalSoundsLevel() == 82);

    nvram::NvRam::saveVmpcSettings(mpc);

    controls->turnWheel(-100);
    controls->up();
    controls->turnWheel(-1);
    controls->up();
    controls->turnWheel(-1);
    for (const auto group : groups)
    {
        mpc.getEngineHost()->setPhysicalSoundGroupLevel(group, 0);
    }

    nvram::NvRam::loadVmpcSettings(mpc);

    REQUIRE(settings->arePhysicalSoundsEnabled());
    REQUIRE(settings->getPhysicalSoundsMixMode() == 1);
    REQUIRE(settings->getPhysicalSoundsLevel() == 82);
    REQUIRE(mpc.getEngineHost()->arePhysicalSoundsEnabled());
    REQUIRE(mpc.getEngineHost()->getPhysicalSoundsMixMode() ==
            engine::PhysicalSoundsMixMode::Dedicated);
    REQUIRE(mpc.getEngineHost()->getPhysicalSoundsLevel() == 82);
    for (size_t i = 0; i < groups.size(); ++i)
    {
        REQUIRE(mpc.getEngineHost()->getPhysicalSoundGroupLevel(groups[i]) ==
                groupLevels[i]);
    }
}

TEST_CASE("Physical settings rows open the group-level window",
          "[vmpc-settings][physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::VmpcSettingsScreen);

    auto controls = mpc.getScreen();
    controls->openWindow();
    REQUIRE(layeredScreen->getCurrentScreenId() ==
            ScreenId::VmpcSettingsScreen);

    for (int i = 0; i < 5; ++i)
    {
        controls->down();
    }
    for (int physicalSetting = 0; physicalSetting < 3; ++physicalSetting)
    {
        controls->openWindow();
        REQUIRE(layeredScreen->getCurrentScreenId() ==
                ScreenId::VmpcPhysicalSoundsScreen);
        controls = mpc.getScreen();
        controls->openWindow();
        REQUIRE(layeredScreen->getCurrentScreenId() ==
                ScreenId::VmpcSettingsScreen);
        controls = mpc.getScreen();
        if (physicalSetting < 2)
        {
            controls->down();
        }
    }
}

TEST_CASE("Physical sound group window edits and clamps every group",
          "[vmpc-settings][physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::VmpcPhysicalSoundsScreen);
    layeredScreen->Draw();
    const auto pixels = layeredScreen->getPixels();
    for (int x = 46; x <= 206; ++x)
    {
        REQUIRE((*pixels)[x][0]);
        REQUIRE((*pixels)[x][8]);
    }
    REQUIRE_FALSE((*pixels)[45][8]);
    REQUIRE_FALSE((*pixels)[207][8]);

    auto controls = mpc.getScreen();
    const auto engineHost = mpc.getEngineHost();

    controls->turnWheel(-200); // Buttons
    REQUIRE(engineHost->getPhysicalSoundGroupLevel(
                engine::PhysicalSoundGroup::Buttons) == 0);
    controls->right();
    controls->turnWheel(-10); // Pads
    REQUIRE(engineHost->getPhysicalSoundGroupLevel(
                engine::PhysicalSoundGroup::Pads) == 90);
    controls->down();
    controls->turnWheel(-20); // Data wheel
    REQUIRE(engineHost->getPhysicalSoundGroupLevel(
                engine::PhysicalSoundGroup::DataWheel) == 80);
    controls->left();
    controls->turnWheel(-30); // Slider
    REQUIRE(engineHost->getPhysicalSoundGroupLevel(
                engine::PhysicalSoundGroup::Slider) == 70);
    controls->down();
    controls->turnWheel(200); // Power
    REQUIRE(engineHost->getPhysicalSoundGroupLevel(
                engine::PhysicalSoundGroup::Power) == 100);
}

TEST_CASE("Legacy physical sound settings default new group levels to 100",
          "[vmpc-settings][physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    std::vector<char> legacyBytes(16, 0);
    legacyBytes[13] = 1;
    legacyBytes[14] = 1;
    legacyBytes[15] = 77;
    REQUIRE(set_file_data(mpc.paths->vmpcSpecificConfigPath(), legacyBytes));

    nvram::NvRam::loadVmpcSettings(mpc);

    const auto engineHost = mpc.getEngineHost();
    REQUIRE(engineHost->arePhysicalSoundsEnabled());
    REQUIRE(engineHost->getPhysicalSoundsMixMode() ==
            engine::PhysicalSoundsMixMode::Dedicated);
    REQUIRE(engineHost->getPhysicalSoundsLevel() == 77);
    for (const auto group :
         {engine::PhysicalSoundGroup::Buttons, engine::PhysicalSoundGroup::Pads,
          engine::PhysicalSoundGroup::Slider,
          engine::PhysicalSoundGroup::DataWheel,
          engine::PhysicalSoundGroup::Power})
    {
        REQUIRE(engineHost->getPhysicalSoundGroupLevel(group) == 100);
    }
}

TEST_CASE("VmpcSettings first scroll preserves the function key strip",
          "[vmpc-settings]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::VmpcSettingsScreen);
    layeredScreen->Draw();

    const auto controls = mpc.getScreen();
    for (int i = 0; i < 4; ++i)
    {
        controls->down();
        layeredScreen->Draw();
    }

    const auto functionKeyPixels = [&]
    {
        std::vector<bool> result;
        for (int x = 0; x < 248; ++x)
        {
            for (int y = 51; y < 60; ++y)
            {
                result.push_back((*layeredScreen->getPixels())[x][y]);
            }
        }
        return result;
    };

    const auto beforeScroll = functionKeyPixels();
    controls->down();
    layeredScreen->Draw();

    REQUIRE(functionKeyPixels() == beforeScroll);
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
