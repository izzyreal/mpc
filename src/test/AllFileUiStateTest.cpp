#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/Mpc2000XlAllFileScreen.hpp"
#include "lcdgui/screens/window/SaveAllFileScreen.hpp"
#include "StrUtil.hpp"

#include <cmrc/cmrc.hpp>
#include <algorithm>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

namespace
{
    constexpr auto kAllResourcePath = "test/AllFileUi/FOO.ALL";
    constexpr auto kCompatibleAllFileName = "FOO.ALL";

    void prepareAllResources(Mpc &mpc)
    {
        auto disk = mpc.getDisk();
        auto fs = cmrc::mpctest::get_filesystem();

        auto file = fs.open(kAllResourcePath);
        std::vector data(file.begin(), file.end());
        auto newFile = disk->newFile(kCompatibleAllFileName);
        newFile->setFileData(data);

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

    const auto allFile = mpc.getDisk()->getFile(kCompatibleAllFileName);
    REQUIRE(allFile);

    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto allFileIt =
        std::find_if(fileNames.begin(), fileNames.end(),
                     [&](const std::string &fileName)
                     {
                         return StrUtil::eqIgnoreCase(fileName,
                                                      allFile->getName());
                     });
    REQUIRE(allFileIt != fileNames.end());

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    loadScreen->setFileLoad(
        static_cast<int>(std::distance(fileNames.begin(), allFileIt)));

    const auto allFileScreen =
        mpc.screens->get<ScreenId::Mpc2000XlAllFileScreen>();
    allFileScreen->function(4);

    REQUIRE(saveAllFileScreen->getFileName() ==
            allFile->getNameWithoutExtension());
}
