#include "catch2/catch_test_macros.hpp"

#include "Mpc.hpp"
#include "AutoSave.hpp"
#include "Paths.hpp"
#include "disk/Volume.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui::screens;

TEST_CASE("Load an empty auto-save state", "[auto-save]")
{
    auto tmpDocsPath = fs::temp_directory_path();
    tmpDocsPath.concat("VMPC2000XL-test");

    {
        Mpc mpc;
        mpc.init(1, 5);

        mpc::AutoSave::restoreAutoSavedState(mpc, tmpDocsPath.string());
        mpc::AutoSave::storeAutoSavedState(mpc, tmpDocsPath.string());
    }

    {
        Mpc mpc;
        mpc.init(1, 5);

        auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");

        vmpcAutoSaveScreen->setAutoLoadOnStart(2);

        REQUIRE_NOTHROW(mpc::AutoSave::restoreAutoSavedState(mpc, tmpDocsPath.string()));
    }
}
