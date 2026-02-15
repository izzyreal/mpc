#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "file/all/SequenceNames.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;

namespace
{
const std::string kSeq07Seq08RegressionFilename = "SEQ0708R.ALL";

void deleteSeq07Seq08RegressionAllFile(const std::shared_ptr<AbstractDisk> &disk)
{
    disk->initFiles();

    for (int i = 0; i < disk->getAllFiles().size(); i++)
    {
        if (disk->getFileName(i) == kSeq07Seq08RegressionFilename)
        {
            (void)disk->getFile(i)->del();
            disk->flush();
            disk->initFiles();
            break;
        }
    }
}

void saveAndReloadAllFile(Mpc &mpc)
{
    const auto disk = mpc.getDisk();
    const auto file = disk->newFile(kSeq07Seq08RegressionFilename);

    file::all::AllParser allParser(mpc);
    file->setFileData(allParser.getBytes());
    disk->flush();
    disk->initFiles();

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    sequencer->deleteAllSequences();
    stateManager->drainQueue();

    for (int i = 0; i < 20; i++)
    {
        sequencer->deleteSong(i);
    }
    stateManager->drainQueue();

    AllLoader::loadEverythingFromFile(
        mpc, disk->getFile(kSeq07Seq08RegressionFilename).get());
}
} // namespace

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

TEST_CASE("Load ALL and check sequence 21 is used", "[load-all]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareAllResources(mpc);

    auto disk = mpc.getDisk();
    auto allFile = disk->getFile("ShouldLoadSeq21.ALL");

    REQUIRE(allFile);

    file::all::AllParser parser(mpc, allFile->getBytes());

    auto seqNames = parser.getSeqNames();

    REQUIRE(seqNames->getNames()[20] == "Sequence21      ");
    REQUIRE(seqNames->getUsednesses()[20]);

    AllLoader::loadEverythingFromAllParser(mpc, parser);

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto seq1 = mpc.getSequencer()->getSequence(0);
    REQUIRE(seq1);
    REQUIRE(seq1->isUsed());

    auto seq21 = mpc.getSequencer()->getSequence(20);
    REQUIRE(seq21);
    REQUIRE(seq21->isUsed());
}

TEST_CASE("Save and reload ALL keeps Seq-07 and Seq-08 used", "[load-all]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto disk = mpc.getDisk();

    deleteSeq07Seq08RegressionAllFile(disk);

    auto seq07 = sequencer->getSequence(6);
    auto seq08 = sequencer->getSequence(7);

    seq07->init(1);
    seq08->init(1);
    seq07->setName("SEQ-07");
    seq08->setName("SEQ-08");
    stateManager->drainQueue();

    saveAndReloadAllFile(mpc);
    stateManager->drainQueue();

    seq07 = sequencer->getSequence(6);
    seq08 = sequencer->getSequence(7);

    REQUIRE(seq07->isUsed());
    REQUIRE(seq08->isUsed());
    REQUIRE(seq07->getName() == "SEQ-07");
    REQUIRE(seq08->getName() == "SEQ-08");

    deleteSeq07Seq08RegressionAllFile(disk);
}
