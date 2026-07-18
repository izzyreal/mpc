#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASequenceFromAllScreen.hpp"
#include "lcdgui/screens/window/Mpc2000XlAllFileScreen.hpp"
#include "lcdgui/screens/window/SaveAllFileScreen.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"
#include "StrUtil.hpp"

#include <cmrc/cmrc.hpp>
#include <algorithm>
#include <vector>

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
    constexpr auto kMpc3000AllResourcePath = "test/RealMpc3000/All/M3K_2SEQ.ALL";
    constexpr auto kMpc3000AllFileName = "M3K_2SEQ.ALL";
    constexpr auto kMpc3000AllSongResourcePath = "test/RealMpc3000/All/M3K_2SEQ1SONG.ALL";
    constexpr auto kMpc3000AllSongFileName = "M3K_2SEQ1SONG.ALL";
    constexpr auto kMpc3000AllEmptyResourcePath = "test/RealMpc3000/All/M3K_EMPTY2.ALL";
    constexpr auto kMpc3000AllEmptyFileName = "M3K_EMPTY2.ALL";
    constexpr auto kMpc60AllResourcePath = "test/RealMpc60/All/MPC60_V214_ALL_SEQS.ALL";
    constexpr auto kMpc60AllFileName = "M60SEQ.ALL";
    constexpr auto kMpc60AllSongResourcePath = "test/RealMpc60/All/MPC60_V214_SONG_2REALSTEPS.ALL";
    constexpr auto kMpc60AllSongFileName = "M60SONG.ALL";

    void addResourceFile(Mpc &mpc, const std::string& resourcePath,
                         const std::string& fileName)
    {
        auto disk = mpc.getDisk();
        auto fs = cmrc::mpctest::get_filesystem();

        auto file = fs.open(resourcePath);
        std::vector data(file.begin(), file.end());
        auto newFile = disk->newFile(fileName);
        newFile->setFileData(data);
    }

    void prepareAllResources(Mpc &mpc)
    {
        addResourceFile(mpc, kAllResourcePath, kCompatibleAllFileName);
        addResourceFile(mpc, kMpc3000AllResourcePath, kMpc3000AllFileName);
        addResourceFile(mpc, kMpc3000AllSongResourcePath, kMpc3000AllSongFileName);
        addResourceFile(mpc, kMpc3000AllEmptyResourcePath, kMpc3000AllEmptyFileName);
        addResourceFile(mpc, kMpc60AllResourcePath, kMpc60AllFileName);
        addResourceFile(mpc, kMpc60AllSongResourcePath, kMpc60AllSongFileName);
        auto disk = mpc.getDisk();
        disk->initFiles();
    }

    int findFileLoadIndex(Mpc &mpc, const std::string& fileName)
    {
        const auto fileNames = mpc.getDisk()->getFileNames();
        const auto it =
            std::find_if(fileNames.begin(), fileNames.end(),
                         [&](const std::string &candidate)
                         {
                             return StrUtil::eqIgnoreCase(candidate, fileName);
                         });
        REQUIRE(it != fileNames.end());
        return static_cast<int>(std::distance(fileNames.begin(), it));
    }

    std::vector<std::vector<bool>>
    renderAllFileScreenFor(Mpc &mpc, const std::string& fileName)
    {
        const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
        loadScreen->setFileLoad(findFileLoadIndex(mpc, fileName));

        const auto allFileScreen =
            mpc.screens->get<ScreenId::Mpc2000XlAllFileScreen>();
        allFileScreen->open();

        std::vector pixels(248, std::vector<bool>(60));
        allFileScreen->drawRecursive(&pixels);
        return pixels;
    }

    void openAllWindowFor(Mpc &mpc, const std::string& fileName)
    {
        auto layeredScreen = mpc.getLayeredScreen();
        layeredScreen->openScreen("load");

        const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
        loadScreen->setFileLoad(findFileLoadIndex(mpc, fileName));
        loadScreen->function(5);

        REQUIRE(layeredScreen->getCurrentScreenName() == "mpc2000xl-all-file");
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

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    loadScreen->setFileLoad(findFileLoadIndex(mpc, kCompatibleAllFileName));

    const auto allFileScreen =
        mpc.screens->get<ScreenId::Mpc2000XlAllFileScreen>();
    allFileScreen->function(4);

    REQUIRE(saveAllFileScreen->getFileName() ==
            allFile->getNameWithoutExtension());
}

TEST_CASE("ALL window title follows the selected ALL file variant",
          "[load-all][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareAllResources(mpc);

    const auto mpc2000xlPixels =
        renderAllFileScreenFor(mpc, kCompatibleAllFileName);
    const auto mpc3000Pixels =
        renderAllFileScreenFor(mpc, kMpc3000AllFileName);

    REQUIRE(mpc2000xlPixels != mpc3000Pixels);
}

TEST_CASE("MPC3000 ALL <SEQ> branch lets the user browse embedded sequences and discard",
          "[load-all][ui][real-mpc3000]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(0)->setName("DEST_A");
    sequencer->getSequence(1)->init(0);
    sequencer->getSequence(1)->setName("DEST_B");
    stateManager->drainQueue();

    openAllWindowFor(mpc, kMpc3000AllFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-from-all");

    const auto seqFromAllScreen =
        mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
    REQUIRE(seqFromAllScreen->findField("file")->getText() == "01");
    REQUIRE(seqFromAllScreen->findLabel("file0")->getText() == "-SEQ01");
    REQUIRE(seqFromAllScreen->findField("load-into")->getText() == "01");
    REQUIRE(seqFromAllScreen->findLabel("load-into0")->getText() == "-DEST_A");

    seqFromAllScreen->turnWheel(1);
    REQUIRE(seqFromAllScreen->findField("file")->getText() == "02");
    REQUIRE(seqFromAllScreen->findLabel("file0")->getText() == "-SEQ02");

    seqFromAllScreen->down();
    REQUIRE(seqFromAllScreen->getFocusedFieldNameOrThrow() == "load-into");
    seqFromAllScreen->turnWheel(1);
    REQUIRE(seqFromAllScreen->findField("load-into")->getText() == "02");
    REQUIRE(seqFromAllScreen->findLabel("load-into0")->getText() == "-DEST_B");

    seqFromAllScreen->function(3);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
    REQUIRE(sequencer->getSequence(1)->getName() == "DEST_B");
}

TEST_CASE("MPC3000 ALL <SEQ> KEEP loads the chosen embedded sequence into the chosen slot",
          "[load-all][ui][real-mpc3000]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(0)->setName("DEST_A");
    sequencer->getSequence(1)->init(0);
    sequencer->getSequence(1)->setName("DEST_B");
    stateManager->drainQueue();

    openAllWindowFor(mpc, kMpc3000AllFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-from-all");

    const auto seqFromAllScreen =
        mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
    seqFromAllScreen->turnWheel(1);
    seqFromAllScreen->down();
    seqFromAllScreen->turnWheel(1);

    seqFromAllScreen->function(4);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
    auto loadedSequence = sequencer->getSequence(1);
    REQUIRE(loadedSequence->isUsed());
    REQUIRE(loadedSequence->getName() == "SEQ02");
    REQUIRE(loadedSequence->getTrack(0)->getEvents().size() == 4);
}

TEST_CASE("MPC3000 ALL <SEQ> PLAY previews the chosen embedded sequence from temp",
          "[load-all][ui][real-mpc3000]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);
    sequencer->getSequence(5)->init(0);
    stateManager->drainQueue();
    sequencer->setSelectedSequenceIndex(SequenceIndex(5), true);
    stateManager->drainQueue();

    openAllWindowFor(mpc, kMpc3000AllFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-from-all");

    const auto seqFromAllScreen =
        mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
    seqFromAllScreen->turnWheel(1);
    seqFromAllScreen->function(2);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-play");
    REQUIRE(sequencer->getSelectedSequenceIndex() == TempSequenceIndex);
    REQUIRE(sequencer->getSequence(TempSequenceIndex)->getName() == "SEQ02");
    REQUIRE(sequencer->getTransport()->isPlaying());
}

TEST_CASE("MPC3000 ALL <SEQ> KEEP loads empty embedded sequences without phantom events",
          "[load-all][ui][real-mpc3000]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(1);
    sequencer->getSequence(0)->setName("DEST_A");
    stateManager->drainQueue();

    openAllWindowFor(mpc, kMpc3000AllEmptyFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-from-all");

    const auto seqFromAllScreen =
        mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
    REQUIRE(seqFromAllScreen->findField("file")->getText() == "01");
    REQUIRE(seqFromAllScreen->findLabel("file0")->getText() == "-SEQ01");
    seqFromAllScreen->turnWheel(1);
    REQUIRE(seqFromAllScreen->findField("file")->getText() == "02");
    REQUIRE(seqFromAllScreen->findLabel("file0")->getText() == "-SEQ02");

    seqFromAllScreen->function(4);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
    auto loadedSequence = sequencer->getSequence(0);
    REQUIRE(loadedSequence->isUsed());
    REQUIRE(loadedSequence->getName() == "SEQ02");
    REQUIRE(loadedSequence->getLastBarIndex() == 0);
    REQUIRE(loadedSequence->getTrack(0)->getEvents().empty());
}

TEST_CASE("MPC3000 ALL LOAD imports sequences and songs through the real UI path",
          "[load-all][ui][real-mpc3000]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    openAllWindowFor(mpc, kMpc3000AllSongFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(4);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
    REQUIRE(sequencer->getSequence(0)->isUsed());
    REQUIRE(sequencer->getSequence(0)->getName() == "SEQ01");
    REQUIRE(sequencer->getSequence(1)->isUsed());
    REQUIRE(sequencer->getSequence(1)->getName() == "SEQ02");

    auto song0 = sequencer->getSong(0);
    REQUIRE(song0->isUsed());
    REQUIRE(song0->getName() == "SONG01");
    REQUIRE(song0->getStepCount() == 2);
}

TEST_CASE("MPC60 v2 ALL <SEQ> branch lets the user browse embedded sequences and keep",
          "[load-all][ui][real-mpc60]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(0)->setName("DEST_A");
    stateManager->drainQueue();

    openAllWindowFor(mpc, kMpc60AllFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-from-all");

    const auto seqFromAllScreen =
        mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
    REQUIRE(seqFromAllScreen->findField("file")->getText() == "01");
    REQUIRE(seqFromAllScreen->findLabel("file0")->getText() == "-SEQ01");

    seqFromAllScreen->function(4);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
    auto loadedSequence = sequencer->getSequence(0);
    REQUIRE(loadedSequence->isUsed());
    REQUIRE(loadedSequence->getName() == "SEQ01");
}

TEST_CASE("MPC60 v2 ALL <SEQ> PLAY previews the chosen embedded sequence from temp",
          "[load-all][ui][real-mpc60]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);
    sequencer->getSequence(5)->init(0);
    stateManager->drainQueue();
    sequencer->setSelectedSequenceIndex(SequenceIndex(5), true);
    stateManager->drainQueue();

    openAllWindowFor(mpc, kMpc60AllFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(2);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-from-all");

    const auto seqFromAllScreen =
        mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();
    seqFromAllScreen->function(2);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-play");
    REQUIRE(sequencer->getSelectedSequenceIndex() == TempSequenceIndex);
    REQUIRE(sequencer->getSequence(TempSequenceIndex)->getName() == "SEQ01");
    REQUIRE(sequencer->getTransport()->isPlaying());
}

TEST_CASE("MPC60 v2 ALL LOAD imports sequences and songs through the real UI path",
          "[load-all][ui][real-mpc60]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    prepareAllResources(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    openAllWindowFor(mpc, kMpc60AllSongFileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->getCurrentScreen()->function(4);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
    REQUIRE(sequencer->getSequence(0)->isUsed());
    REQUIRE(sequencer->getSequence(0)->getName() == "SEQ01");

    auto song0 = sequencer->getSong(0);
    REQUIRE(song0->isUsed());
    REQUIRE(song0->getName() == "SONG01");
    REQUIRE(song0->getStepCount() == 2);
}
