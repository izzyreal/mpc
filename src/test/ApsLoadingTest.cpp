#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "sampler/Sampler.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/ApsLoader.hpp"

#include <cmrc/cmrc.hpp>
#include <string_view>

#include "engine/EngineHost.hpp"

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui::screens::window;

void prepareApsResources(Mpc &mpc)
{
    auto disk = mpc.getDisk();

    auto fs = cmrc::mpctest::get_filesystem();

    for (auto &&entry : fs.iterate_directory("test/ApsLoading"))
    {
        auto file = fs.open("test/ApsLoading/" + entry.filename());
        char *data =
            (char *)std::string_view(file.begin(), file.end() - file.begin())
                .data();
        auto newFile = disk->newFile(entry.filename());
        std::vector dataVec(data, data + file.size());
        newFile->setFileData(dataVec);
    }

    disk->initFiles();
}

void doApsTest(Mpc &mpc)
{
    prepareApsResources(mpc);

    auto disk = mpc.getDisk();
    auto apsFile = disk->getFile("ALL_PGMS.APS");

    constexpr bool headless = true;
    ApsLoader::load(mpc, apsFile, headless);
    mpc.getEngineHost()->applyPendingStateChanges();

    auto p1 = mpc.getSampler()->getProgram(0);
    auto p2 = mpc.getSampler()->getProgram(1);

    REQUIRE(mpc.getSampler()->getProgramCount() == 2);
    REQUIRE(p1->getName() == "PROGRAM1");
    REQUIRE(p2->getName() == "PROGRAM2");

    REQUIRE(mpc.getSampler()->getSoundCount() == 3);
    REQUIRE(mpc.getSampler()->getSoundName(0) == "sound1");
    REQUIRE(mpc.getSampler()->getSoundName(1) == "sound2");
    REQUIRE(mpc.getSampler()->getSoundName(2) == "sound3");

    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 2);
}

TEST_CASE("Load APS with 2 programs and 3 sounds", "[load-aps]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    doApsTest(mpc);
}
