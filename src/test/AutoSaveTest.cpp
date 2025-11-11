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
        const auto autosaveDir = mpc.paths->autoSavePath();
        auto saveTarget = std::make_shared<DirectorySaveTarget>(autosaveDir);
        AutoSave::restoreAutoSavedState(mpc, saveTarget, isHeadless);
        AutoSave::storeAutoSavedState(mpc, saveTarget);
    }

    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);

        auto vmpcAutoSaveScreen =
            mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();

        vmpcAutoSaveScreen->setAutoLoadOnStart(2);
        const auto autosaveDir = mpc.paths->autoSavePath();
        auto saveTarget = std::make_shared<DirectorySaveTarget>(autosaveDir);
        REQUIRE_NOTHROW(
            mpc::AutoSave::restoreAutoSavedState(mpc, saveTarget, isHeadless));
    }
}
