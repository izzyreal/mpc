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

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;

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

    auto seq1 = mpc.getSequencer()->getSequence(0);
    REQUIRE(seq1);
    REQUIRE(seq1->isUsed());

    auto seq21 = mpc.getSequencer()->getSequence(20);
    REQUIRE(seq21);
    REQUIRE(seq21->isUsed());
}
