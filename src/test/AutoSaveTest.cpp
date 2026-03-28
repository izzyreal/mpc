#include "DirectorySaveTarget.hpp"
#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "AutoSave.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

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
