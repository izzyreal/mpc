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
        mpc.init(1, 5);

        mpc::AutoSave::restoreAutoSavedState(mpc);
        mpc::AutoSave::storeAutoSavedState(mpc);
    }

    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        mpc.init(1, 5);

        auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");

        vmpcAutoSaveScreen->setAutoLoadOnStart(2);

        REQUIRE_NOTHROW(mpc::AutoSave::restoreAutoSavedState(mpc));
    }
}
