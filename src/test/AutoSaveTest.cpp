#include "DirectorySaveTarget.hpp"
#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "AutoSave.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui::screens;

TEST_CASE("Load an empty auto-save state", "[auto-save]")
{
    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        const auto autosaveDir = mpc.paths->autoSavePath();
        auto saveTarget = std::make_shared<mpc::DirectorySaveTarget>(autosaveDir);
        mpc::AutoSave::restoreAutoSavedStateWithTarget(mpc, saveTarget);
        mpc::AutoSave::storeAutoSavedStateWithTarget(mpc, saveTarget);

    }

    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);

        auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>();

        vmpcAutoSaveScreen->setAutoLoadOnStart(2);
        const auto autosaveDir = mpc.paths->autoSavePath();
        auto saveTarget = std::make_shared<mpc::DirectorySaveTarget>(autosaveDir);
        REQUIRE_NOTHROW(mpc::AutoSave::restoreAutoSavedStateWithTarget(mpc, saveTarget));
    }
}
