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

    void prepareSetFile(Mpc &mpc)
    {
        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open("test/RealMpc60/Set/ROCK.SET");
        std::vector<char> data(file.begin(), file.end());
        auto newFile = mpc.getDisk()->newFile(kSetFileName);
        newFile->setFileData(data);
        mpc.getDisk()->initFiles();
    }

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
        const auto start = std::chrono::steady_clock::now();

        while (mpc.getLayeredScreen()->getCurrentScreenName() != "load")
        {
            REQUIRE(std::chrono::steady_clock::now() - start < timeout);
            mpc.getEngineHost()->prepareProcessBlock(512);
            mpc.getLayeredScreen()->timerCallback();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void waitForTimedPopup(Mpc &mpc)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        mpc.getLayeredScreen()->timerCallback();
    }
}

TEST_CASE(".SET file opens discovered UI flow", "[load-set][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareSetFile(mpc);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto setFileIt =
        std::find_if(fileNames.begin(), fileNames.end(),
                     [](const std::string &fileName)
                     {
                         return StrUtil::eqIgnoreCase(fileName, kSetFileName);
                     });
    REQUIRE(setFileIt != fileNames.end());
    loadScreen->setFileLoad(
        static_cast<int>(std::distance(fileNames.begin(), setFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set");

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "conversion-table");

    layeredScreen->getCurrentScreen()->function(3);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load");

    loadScreen->function(5);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set");

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "conversion-table");

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set-replace-add");
    REQUIRE(layeredScreen->getCurrentScreen()
                ->findField("load-replace-sound")
                ->getText() == "NO(FASTER)");

    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()
                ->findField("load-replace-sound")
                ->getText() == "YES");

    layeredScreen->getCurrentScreen()->turnWheel(-1);
    REQUIRE(layeredScreen->getCurrentScreen()
                ->findField("load-replace-sound")
                ->getText() == "NO(FASTER)");

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "popup");
    waitForLoadScreen(mpc);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load");

    loadScreen->function(5);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set");

    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set-sound");
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT CLSD (A01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:HAT2CLSD        ");

    layeredScreen->getCurrentScreen()->function(3);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set");

    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set-sound");

    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT MEDM (A01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:HAT2MED         ");

    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT OPEN (A01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:HAT2OPN         ");

    layeredScreen->getCurrentScreen()->turnWheel(15);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "PRC4      (A16)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:BIG_CLAP        ");

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "popup");
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sound");
    REQUIRE(mpc.getSampler()->getPreviewSound()->getName() == "BIG_CLAP");
    REQUIRE(mpc.getSampler()->getPreviewSound()->getSampleRate() == 40000);
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
    waitForTimedPopup(mpc);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
}

TEST_CASE(".SET conversion table resolves pad names through DRUM1 program",
          "[load-set][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareSetFile(mpc);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("vmpc-settings");
    REQUIRE(layeredScreen->setFocus("initial-pad-mapping"));
    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()
                ->findField("initial-pad-mapping")
                ->getText() == "ORIGINAL");

    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto setFileIt =
        std::find_if(fileNames.begin(), fileNames.end(),
                     [](const std::string &fileName)
                     {
                         return StrUtil::eqIgnoreCase(fileName, kSetFileName);
                     });
    REQUIRE(setFileIt != fileNames.end());
    loadScreen->setFileLoad(
        static_cast<int>(std::distance(fileNames.begin(), setFileIt)));
    loadScreen->function(5);

    auto drum1Program =
        mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    REQUIRE(drum1Program);
    drum1Program->initPadAssign();
    mpc.getSequencer()->getDrumBus(mpc::sequencer::BusType::DRUM1)->setProgramIndex(
        drum1Program->getProgramIndex());
    mpc.getEngineHost()->prepareProcessBlock(512);
    layeredScreen->timerCallback();
    REQUIRE(drum1Program->getNoteFromPad(ProgramPadIndex(0)) == DrumNoteNumber(37));
    REQUIRE(drum1Program->getNoteFromPad(ProgramPadIndex(1)) == DrumNoteNumber(36));
    REQUIRE(drum1Program->getNoteFromPad(ProgramPadIndex(2)) == DrumNoteNumber(42));
    REQUIRE(drum1Program->getNoteFromPad(ProgramPadIndex(3)) == DrumNoteNumber(82));

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "conversion-table");

    layeredScreen->getCurrentScreen()->open();
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT CLSD (A01)   ");
    REQUIRE(layeredScreen->getCurrentScreen()->findField("becomes-note")->getText() ==
            "42/A03");

    mpc.screens->get<ScreenId::LoadASetScreen>()->setConversionTargetNote(0, DrumNoteNumber(82));
    layeredScreen->getCurrentScreen()->open();
    REQUIRE(layeredScreen->getCurrentScreen()->findField("becomes-note")->getText() ==
            "82/A04");
}

TEST_CASE(".SET single-sound browser keeps unassigned UK-8 pads visible",
          "[load-set][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc60/Set/UK-8.SET");
    std::vector<char> data(file.begin(), file.end());
    auto newFile = mpc.getDisk()->newFile("UK-8.SET");
    newFile->setFileData(data);
    mpc.getDisk()->initFiles();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto setFileIt =
        std::find_if(fileNames.begin(), fileNames.end(),
                     [](const std::string &fileName)
                     {
                         return StrUtil::eqIgnoreCase(fileName, "UK-8.SET");
                     });
    REQUIRE(setFileIt != fileNames.end());
    loadScreen->setFileLoad(
        static_cast<int>(std::distance(fileNames.begin(), setFileIt)));
    loadScreen->function(5);

    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set-sound");
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT CLSD (A01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:S7_HH_CL        ");

    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT MEDM (A01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:S7_HH_MD        ");

    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "HIHT OPEN (A01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:S7_HH_OP        ");

    layeredScreen->getCurrentScreen()->turnWheel(1);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "SNR1      (A02)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:UK-SN31         ");

    layeredScreen->getCurrentScreen()->turnWheel(15);
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "DR01      (B01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:(no assign)");

    layeredScreen->getCurrentScreen()->function(4);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set-sound");
    REQUIRE(layeredScreen->getCurrentScreen()->findField("mpc60-pad")->getText() ==
            "DR01      (B01)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:(no assign)");

    layeredScreen->getCurrentScreen()->turnWheel(7);
    REQUIRE(layeredScreen->getCurrentScreen()
                ->findField("mpc60-pad")
                ->getText() == "DR08      (B08)");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:(no assign)");
}
