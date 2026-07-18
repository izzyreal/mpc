#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASetScreen.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "StrUtil.hpp"

#include <cmrc/cmrc.hpp>
#include <algorithm>
#include <chrono>
#include <thread>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

namespace
{
    constexpr auto kSetFileName = "ROCK.SET";

    void prepareBrokenSetFile(Mpc &mpc)
    {
        auto newFile = mpc.getDisk()->newFile("BROKEN.SET");
        std::vector<char> data{'B', 'A', 'D'};
        newFile->setFileData(data);
        mpc.getDisk()->initFiles();
    }

    void waitForLoadScreen(Mpc &mpc)
    {
        constexpr auto timeout = std::chrono::seconds(5);
        const auto deadline = std::chrono::steady_clock::now() + timeout;

        while (mpc.getLayeredScreen()->getCurrentScreenName() != "load" &&
               std::chrono::steady_clock::now() < deadline)
        {
            mpc.getEngineHost()->prepareProcessBlock(512);
            mpc.getLayeredScreen()->timerCallback();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load");
    }

    void waitForScreen(Mpc &mpc, const std::string &screenName)
    {
        constexpr auto timeout = std::chrono::seconds(5);
        const auto deadline = std::chrono::steady_clock::now() + timeout;

        while (mpc.getLayeredScreen()->getCurrentScreenName() != screenName &&
               std::chrono::steady_clock::now() < deadline)
        {
            mpc.getLayeredScreen()->timerCallback();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == screenName);
    }
}

TEST_CASE("Unreadable .SET file reports error and returns to LOAD",
          "[load-set][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareBrokenSetFile(mpc);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto setFileIt =
        std::find_if(fileNames.begin(), fileNames.end(),
                     [](const std::string &fileName)
                     {
                         return StrUtil::eqIgnoreCase(fileName, "BROKEN.SET");
                     });
    REQUIRE(setFileIt != fileNames.end());
    loadScreen->setFileLoad(
        static_cast<int>(std::distance(fileNames.begin(), setFileIt)));
    loadScreen->function(5);

    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getCurrentScreenName() == "popup");
    waitForScreen(mpc, "load");
    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
}

