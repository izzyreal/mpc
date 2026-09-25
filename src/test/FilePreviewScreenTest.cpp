#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "TestMpc.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

namespace
{
    void preparePreviewDirectory(Mpc &mpc)
    {
        TestMpc::initializeTestMpcWithoutIoServices(mpc);
        const auto disk = mpc.getDisk();
        REQUIRE(disk->newFolder("PREVIEW"));
        disk->initFiles();
        REQUIRE(disk->moveForward("PREVIEW"));
        disk->initFiles();
        REQUIRE(disk->getFileNames().empty());
    }

    void openPreviewScreen(Mpc &mpc, const bool directory)
    {
        const auto ls = mpc.getLayeredScreen();
        ls->openScreenById(ScreenId::LoadScreen);
        ls->setFocus("file");
        if (directory)
        {
            ls->openScreenById(ScreenId::DirectoryScreen);
            mpc.screens->get<ScreenId::DirectoryScreen>()->right();
        }
    }

    void preview(Mpc &mpc, const bool directory)
    {
        if (directory)
        {
            mpc.screens->get<ScreenId::DirectoryScreen>()->function(5);
        }
        else
        {
            mpc.screens->get<ScreenId::LoadScreen>()->function(4);
        }
        mpc.getEngineHost()->prepareProcessBlock(512);
        mpc.getLayeredScreen()->timerCallback();
    }

    void checkUnavailablePreview(const bool directory)
    {
        const auto scenario =
            GENERATE("empty", "deleted", "stale", "folder", "unsupported");
        CAPTURE(directory, scenario);
        Mpc mpc;
        preparePreviewDirectory(mpc);
        const auto disk = mpc.getDisk();
        const auto load = mpc.screens->get<ScreenId::LoadScreen>();
        const std::string state = scenario;
        if (state == "folder")
        {
            REQUIRE(disk->newFolder("FOLDER"));
        }
        else if (state != "empty")
        {
            REQUIRE(
                disk->newFile(state == "deleted" ? "SAMPLE.WAV" : "FILE.TXT"));
        }
        openPreviewScreen(mpc, directory);
        load->setFileLoad(0);
        if (state == "deleted")
        {
            REQUIRE(load->getSelectedFile());
            REQUIRE(load->getSelectedFile()->del());
            disk->initFiles();
        }
        else if (state == "stale")
        {
            REQUIRE_FALSE(disk->getFileNames().empty());
            load->setFileLoad(static_cast<int>(disk->getFileNames().size()));
        }
        if (state == "empty" || state == "deleted" || state == "stale")
        {
            REQUIRE_FALSE(load->getSelectedFile());
        }
        const auto screen = mpc.getLayeredScreen()->getCurrentScreenId();
        REQUIRE_FALSE(mpc.getEngineHost()->getSoundPlayer()->isPlaying());
        preview(mpc, directory);
        CHECK(mpc.getLayeredScreen()->getCurrentScreenId() == screen);
        CHECK_FALSE(mpc.getEngineHost()->getSoundPlayer()->isPlaying());
    }
} // namespace

TEST_CASE("LOAD preview ignores missing or unplayable selections",
          "[file-preview][load-preview]")
{
    checkUnavailablePreview(false);
}

TEST_CASE("DIRECTORY preview ignores missing or unplayable selections",
          "[file-preview][directory-preview]")
{
    checkUnavailablePreview(true);
}

TEST_CASE("File preview still starts a valid sample", "[file-preview]")
{
    const auto directory = GENERATE(false, true);
    CAPTURE(directory);
    Mpc mpc;
    preparePreviewDirectory(mpc);
    const auto fixture = cmrc::mpctest::get_filesystem().open(
        "test/RealMpc3000/Snd/SOUND017.SND");
    auto file = mpc.getDisk()->newFile("SAMPLE.SND");
    REQUIRE(file);
    std::vector<char> bytes(fixture.begin(), fixture.end());
    file->setFileData(bytes);
    mpc.getEngineHost()->getAudioServer()->setSampleRate(44100);
    openPreviewScreen(mpc, directory);
    mpc.screens->get<ScreenId::LoadScreen>()->setFileLoad(0);
    preview(mpc, directory);
    CHECK(mpc.getEngineHost()->getSoundPlayer()->isPlaying());
    CHECK(mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::PopupScreen);
}
