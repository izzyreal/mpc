#include "DirectorySaveTarget.hpp"
#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "AutoSave.hpp"
#include "disk/AbstractDisk.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include <condition_variable>
#include <mutex>

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

constexpr bool isHeadless = true;

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
