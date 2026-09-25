#include "DirectorySaveTarget.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include "TestMpc.hpp"
#include "AutoSave.hpp"
#include "disk/AbstractDisk.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"
#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include <condition_variable>
#include <future>
#include <mutex>

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

constexpr bool isHeadless = true;

TEST_CASE("AutoSave restores keyboard settings on first entry",
          "[auto-save][startup][vmpc-keyboard]")
{
    const bool headless = GENERATE(false, true);
    CAPTURE(headless);

    Mpc seed;
    TestMpc::initializeTestMpcWithoutIoServices(seed);
    seed.getLayeredScreen()->openScreenById(ScreenId::VmpcKeyboardScreen);
    seed.screens->get<ScreenId::VmpcAutoSaveScreen>()->setAutoSaveOnExit(1);

    const auto saveTarget = std::make_shared<DirectorySaveTarget>(
        seed.paths->getDocuments()->autoSavePath());
    AutoSave::storeAutoSavedState(seed, saveTarget);
    const auto savedScreen = saveTarget->getFileData("screen.txt");
    REQUIRE(savedScreen);
    REQUIRE(std::string(savedScreen->begin(), savedScreen->end()) ==
            "vmpc-keyboard");

    Mpc restored;
    restored.paths = seed.paths;
    MpcInitOptions options;
    options.detectRawUsbVolumes = false;
    options.installDemoFiles = false;
    options.startMidiDeviceDetector = false;
    options.startAudioServer = false;
    options.fileOperationTimings =
        FileOperationTimings::uniform(std::chrono::milliseconds(1));
    restored.init(options);
    restored.screens->get<ScreenId::VmpcAutoSaveScreen>()->setAutoLoadOnStart(2);

    const auto completion = std::make_shared<std::promise<void>>();
    auto completed = completion->get_future();
    restored.getAutoSave()->restoreAutoSavedState(
        restored, saveTarget, headless,
        [completion]
        {
            completion->set_value();
        });
    REQUIRE(completed.wait_for(std::chrono::seconds(3)) ==
            std::future_status::ready);

    // Non-headless restoration queues loading popups before the saved screen.
    restored.getLayeredScreen()->timerCallback();
    REQUIRE(restored.getLayeredScreen()->getCurrentScreenId() ==
            ScreenId::VmpcKeyboardScreen);
    const auto keyboardScreen =
        restored.screens->get<ScreenId::VmpcKeyboardScreen>();
    REQUIRE_FALSE(keyboardScreen->hasMappingChanged());
    keyboardScreen->turnWheel(1);
    REQUIRE(keyboardScreen->hasMappingChanged());
}

TEST_CASE("Load an empty auto-save state", "[auto-save]")
{
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        const auto autosaveDir = mpc.paths->getDocuments()->autoSavePath();
        auto saveTarget = std::make_shared<DirectorySaveTarget>(autosaveDir);
        mpc.getAutoSave()->restoreAutoSavedState(mpc, saveTarget, isHeadless);
        mpc.getAutoSave()->storeAutoSavedState(mpc, saveTarget);
    }

    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);

        auto vmpcAutoSaveScreen =
            mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();

        vmpcAutoSaveScreen->setAutoLoadOnStart(2);
        const auto autosaveDir = mpc.paths->getDocuments()->autoSavePath();
        auto saveTarget = std::make_shared<DirectorySaveTarget>(autosaveDir);
        REQUIRE_NOTHROW(mpc.getAutoSave()->restoreAutoSavedState(
            mpc, saveTarget, isHeadless));
    }
}

TEST_CASE("AutoSave restore tolerates corrupt persisted files", "[auto-save]")
{
    {
        Mpc seed;
        TestMpc::resetTestDataRoot(seed);
    }

    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    mpc.setPluginModeEnabled(true);

    auto vmpcAutoSaveScreen =
        mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
    vmpcAutoSaveScreen->setAutoLoadOnStart(2);

    const auto autosaveDir = mpc.paths->getDocuments()->autoSavePath();
    auto saveTarget = std::make_shared<DirectorySaveTarget>(autosaveDir);

    REQUIRE(set_file_data(autosaveDir / "screen.txt", std::string("sequencer")));
    REQUIRE(set_file_data(autosaveDir / "currentDir.txt", std::string("/bad")));
    REQUIRE(set_file_data(autosaveDir / "APS.APS",
                          std::vector<char>{'n', 'o', 't', '-', 'a', 'p', 's'}));
    REQUIRE(set_file_data(autosaveDir / "ALL.ALL",
                          std::vector<char>{'n', 'o', 't', '-', 'a', 'l', 'l'}));
    REQUIRE(set_file_data(autosaveDir / "sounds.txt", std::string("BROKEN.SND\n")));
    REQUIRE(set_file_data(autosaveDir / "BROKEN.SND",
                          std::vector<char>{'n', 'o', 't', '-', 's', 'n', 'd'}));

    REQUIRE_NOTHROW(
        mpc.getAutoSave()->restoreAutoSavedState(mpc, saveTarget, true));
}

TEST_CASE("Startup work queued after AutoSave restore keeps its screen",
          "[auto-save][startup]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    const auto autoSaveScreen =
        mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
    autoSaveScreen->setAutoLoadOnStart(2);

    const auto autosaveDir = mpc.paths->getDocuments()->autoSavePath();
    const auto saveTarget =
        std::make_shared<DirectorySaveTarget>(autosaveDir);

    const std::string restoredScreen{"sequencer"};
    REQUIRE(saveTarget->setFileData(
        "screen.txt",
        {restoredScreen.begin(), restoredScreen.end()}));

    const auto currentDir = mpc.getDisk()->getAbsolutePath();
    REQUIRE(saveTarget->setFileData(
        "currentDir.txt", {currentDir.begin(), currentDir.end()}));

    const auto knownControllerScreen =
        mpc.screens->get<ScreenId::VmpcKnownControllerDetectedScreen>();
    knownControllerScreen->setControllerName("MPD218");

    std::mutex completionMutex;
    std::condition_variable completionCondition;
    bool completed = false;

    mpc.getAutoSave()->restoreAutoSavedState(
        mpc, saveTarget, true,
        [&]
        {
            const auto layeredScreen = mpc.getLayeredScreen();
            layeredScreen->postToUiThread(utils::Task(
                [layeredScreen]
                {
                    layeredScreen->openScreenById(
                        ScreenId::VmpcKnownControllerDetectedScreen);
                }));

            {
                const std::lock_guard lock(completionMutex);
                completed = true;
            }
            completionCondition.notify_one();
        });

    {
        std::unique_lock lock(completionMutex);
        REQUIRE(completionCondition.wait_for(
            lock, std::chrono::seconds(2),
            [&]
            {
                return completed;
            }));
    }

    mpc.getLayeredScreen()->timerCallback();
    CHECK(mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::VmpcKnownControllerDetectedScreen);
}
