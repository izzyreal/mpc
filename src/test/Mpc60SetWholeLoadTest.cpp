#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "TestMpc.hpp"
#include "StrUtil.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASetScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include <cmrc/cmrc.hpp>
#include <algorithm>
#include <chrono>
#include <thread>

CMRC_DECLARE(mpctest);

namespace
{
    void prepareSetFile(mpc::Mpc &mpc, const std::string &setFileName)
    {
        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open("test/RealMpc60/Set/" + setFileName);
        std::vector<char> data(file.begin(), file.end());
        auto newFile = mpc.getDisk()->newFile(setFileName);
        newFile->setFileData(data);
        mpc.getDisk()->initFiles();
    }

    void selectSetFile(mpc::Mpc &mpc, const std::string &setFileName)
    {
        auto layeredScreen = mpc.getLayeredScreen();
        layeredScreen->openScreen("load");

        const auto loadScreen =
            mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
        const auto fileNames = mpc.getDisk()->getFileNames();
        const auto setFileIt =
            std::find_if(fileNames.begin(), fileNames.end(),
                         [&setFileName](const std::string &fileName)
                         {
                             return mpc::StrUtil::eqIgnoreCase(fileName,
                                                               setFileName);
                         });
        REQUIRE(setFileIt != fileNames.end());
        loadScreen->setFileLoad(
            static_cast<int>(std::distance(fileNames.begin(), setFileIt)));
        loadScreen->function(5);
        REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-set");
    }

    void waitForLoadScreen(mpc::Mpc &mpc)
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

    void requireSelectedDrumProgramIsUsed(mpc::Mpc &mpc,
                                          const mpc::ProgramIndex expected)
    {
        const auto track = mpc.getSequencer()->getSelectedTrack();
        REQUIRE(mpc::sequencer::isDrumBusType(track->getBusType()));

        const auto drumBus = mpc.getSequencer()->getDrumBus(track->getBusType());
        REQUIRE(drumBus->getProgramIndex() == expected);
        REQUIRE(mpc.getSampler()->getProgram(expected.get())->isUsed());
    }
}

TEST_CASE("MPC60 SET CLEAR load imports sounds and assigns converted notes",
          "[kaitai-set][load-set]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    mpc.getEngineHost()->prepareProcessBlock(512);
    prepareSetFile(mpc, "ROCK.SET");

    auto existing = mpc.getSampler()->addSound();
    REQUIRE(existing != nullptr);
    existing->setName("dummy");

    selectSetFile(mpc, "ROCK.SET");

    const auto setScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASetScreen>();
    const auto hiHatNote = setScreen->getConversionTargetNote(0);
    const auto rideNote = setScreen->getConversionTargetNote(10);

    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "conversion-table");
    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load-a-set-replace-add");
    mpc.getLayeredScreen()->getCurrentScreen()->function(2);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "popup");
    waitForLoadScreen(mpc);

    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load");
    REQUIRE(mpc.getSampler()->getProgramCount() == 1);
    REQUIRE(mpc.getSampler()->getSoundCount() == 34);
    REQUIRE(mpc.getSampler()->getProgram(0)->getName() == "ROCK");
    REQUIRE(mpc.getSampler()->getSoundName(19) == "HAT2CLSD");
    REQUIRE(mpc.getSampler()->getSoundName(16) == "RIDE4B");
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(hiHatNote)->getSoundIndex() == 19);
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(rideNote)->getSoundIndex() == 16);
    requireSelectedDrumProgramIsUsed(mpc, mpc::ProgramIndex(0));
}

TEST_CASE("MPC60 SET default conversion table matches MPC2000XL",
          "[kaitai-set][load-set]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    prepareSetFile(mpc, "ROCK.SET");
    selectSetFile(mpc, "ROCK.SET");

    const auto setScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASetScreen>();

    const std::vector<int> expectedNotes{
        42, 82, 46, 38, 37, 36, 48, 47, 45, 43, 51, 53, 49, 55, 69, 54, 56,
        39, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 70, 71, 72, 73};

    REQUIRE(expectedNotes.size() == 34);

    for (size_t i = 0; i < expectedNotes.size(); ++i)
    {
        REQUIRE(static_cast<int>(
                    setScreen->getConversionTargetNote(static_cast<int>(i))) ==
                expectedNotes[i]);
    }
}

TEST_CASE("MPC60 SET LOAD adds imported program and offsets sound indices",
          "[kaitai-set][load-set]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    mpc.getEngineHost()->prepareProcessBlock(512);
    prepareSetFile(mpc, "ROCK.SET");

    auto existing = mpc.getSampler()->addSound();
    REQUIRE(existing != nullptr);
    existing->setName("dummy");

    selectSetFile(mpc, "ROCK.SET");

    const auto setScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASetScreen>();
    const auto hiHatNote = setScreen->getConversionTargetNote(0);

    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "popup");
    waitForLoadScreen(mpc);

    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load");
    REQUIRE(mpc.getSampler()->getProgramCount() == 2);
    REQUIRE(mpc.getSampler()->getSoundCount() == 35);
    REQUIRE(mpc.getSampler()->getSoundName(0) == "dummy");
    REQUIRE(mpc.getSampler()->getProgram(1)->getName() == "ROCK");
    REQUIRE(mpc.getSampler()->getProgram(1)->getNoteParameters(hiHatNote)->getSoundIndex() == 20);
    requireSelectedDrumProgramIsUsed(mpc, mpc::ProgramIndex(1));
}

TEST_CASE("MPC60 STUDIO SET CLEAR load imports sounds and assigns converted notes",
          "[kaitai-set][load-set]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    mpc.getEngineHost()->prepareProcessBlock(512);
    prepareSetFile(mpc, "STUDIO.SET");

    selectSetFile(mpc, "STUDIO.SET");

    const auto setScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASetScreen>();
    const auto hiHatNote = setScreen->getConversionTargetNote(0);
    const auto rideNote = setScreen->getConversionTargetNote(10);

    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "conversion-table");
    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load-a-set-replace-add");
    mpc.getLayeredScreen()->getCurrentScreen()->function(2);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "popup");
    waitForLoadScreen(mpc);

    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load");
    REQUIRE(mpc.getSampler()->getProgramCount() == 1);
    REQUIRE(mpc.getSampler()->getSoundCount() == 34);
    REQUIRE(mpc.getSampler()->getProgram(0)->getName() == "STUDIO");
    REQUIRE(mpc.getSampler()->getSoundName(19) == "HAT1CLSD");
    REQUIRE(mpc.getSampler()->getSoundName(23) == "RIDE_#1");
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(hiHatNote)->getSoundIndex() == 19);
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(rideNote)->getSoundIndex() == 23);
    requireSelectedDrumProgramIsUsed(mpc, mpc::ProgramIndex(0));
}

TEST_CASE("MPC60 UK-8 SET load skips unassigned source pads and blank entries",
          "[kaitai-set][load-set]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    mpc.getEngineHost()->prepareProcessBlock(512);
    prepareSetFile(mpc, "UK-8.SET");

    selectSetFile(mpc, "UK-8.SET");

    const auto setScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASetScreen>();
    const auto hiHatNote = setScreen->getConversionTargetNote(0);
    const auto unassignedNote = setScreen->getConversionTargetNote(18);
    const auto scratchNote = setScreen->getConversionTargetNote(33);

    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "conversion-table");
    mpc.getLayeredScreen()->getCurrentScreen()->function(4);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load-a-set-replace-add");
    mpc.getLayeredScreen()->getCurrentScreen()->function(2);
    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "popup");
    waitForLoadScreen(mpc);

    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenName() == "load");
    REQUIRE(mpc.getSampler()->getProgramCount() == 1);
    REQUIRE(mpc.getSampler()->getSoundCount() == 26);
    REQUIRE(mpc.getSampler()->getProgram(0)->getName() == "UK-8");
    REQUIRE(mpc.getSampler()->getSoundName(0) == "S7_HH_CL");
    REQUIRE(mpc.getSampler()->getSoundName(20) == "SCRATCH4");
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(hiHatNote)->getSoundIndex() == 0);
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(unassignedNote)->getSoundIndex() == -1);
    REQUIRE(mpc.getSampler()->getProgram(0)->getNoteParameters(scratchNote)->getSoundIndex() == 20);
    requireSelectedDrumProgramIsUsed(mpc, mpc::ProgramIndex(0));
}
