#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/Mpc2000XlAllFileScreen.hpp"
#include "lcdgui/screens/window/SaveAllFileScreen.hpp"

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

namespace
{
    void prepareAllResources(Mpc &mpc)
    {
        auto disk = mpc.getDisk();
        auto fs = cmrc::mpctest::get_filesystem();

        for (auto &&entry : fs.iterate_directory("test/AllLoading"))
        {
            auto file = fs.open("test/AllLoading/" + entry.filename());
            std::vector data(file.begin(), file.end());
            auto newFile = disk->newFile(entry.filename());
            newFile->setFileData(data);
        }

        disk->initFiles();
    }
}

TEST_CASE("Complete ALL load remembers name for later ALL save",
          "[load-all][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareAllResources(mpc);

    const auto saveAllFileScreen = mpc.screens->get<ScreenId::SaveAllFileScreen>();
    saveAllFileScreen->setFileName("OLDNAME");

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    loadScreen->setFileLoad(0);

    const auto allFileScreen =
        mpc.screens->get<ScreenId::Mpc2000XlAllFileScreen>();
    allFileScreen->function(4);

    REQUIRE(saveAllFileScreen->getFileName() == "ShouldLoadSeq21");
}
