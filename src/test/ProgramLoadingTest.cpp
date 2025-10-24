#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/ProgramLoader.hpp"
#include "lcdgui/screens/window/LoadAProgramScreen.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui::screens::window;

void prepareResources(mpc::Mpc &mpc)
{
    auto disk = mpc.getDisk();

    auto fs = cmrc::mpctest::get_filesystem();

    for (auto &&entry : fs.iterate_directory("test/ProgramLoading"))
    {
        assert(entry.is_directory());

        disk->newFolder(entry.filename());
        disk->initFiles();
        disk->moveForward(entry.filename());
        disk->initFiles();

        for (auto &&entry2 : fs.iterate_directory("test/ProgramLoading/" + entry.filename()))
        {
            auto file = fs.open("test/ProgramLoading/" + entry.filename() + "/" + entry2.filename());
            char *data = (char *)std::string_view(file.begin(), file.end() - file.begin()).data();
            auto newFile = disk->newFile(entry2.filename());
            std::vector<char> dataVec(data, data + file.size());
            newFile->setFileData(dataVec);
        }

        disk->moveBack();
        disk->initFiles();
    }
}

void doTest(mpc::Mpc &mpc,
            const bool clear,
            const bool replaceSameSounds,
            std::shared_ptr<sampler::Program> &p1,
            std::shared_ptr<sampler::Program> &p2)
{
    mpc.screens->get<LoadAProgramScreen>()->setLoadReplaceSameSound(replaceSameSounds);

    prepareResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile1 = disk->getFile("PROGRAM1.PGM");

    mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/false);
    mpc.getSampler()->deleteAllSamples();

    p1 = mpc.getSampler()->createNewProgramAddFirstAvailableSlot().lock();
    ProgramLoader::loadProgram(mpc, pgmFile1, p1);

    REQUIRE(mpc.getSampler()->getProgram(0) == p1);
    REQUIRE(mpc.getSampler()->getProgramCount() == 1);

    REQUIRE(mpc.getSampler()->getSoundCount() == 2);
    REQUIRE(mpc.getSampler()->getSoundName(0) == "sound1");
    REQUIRE(mpc.getSampler()->getSoundName(1) == "sound2");
    REQUIRE(mpc.getSampler()->getSound(1)->getEnd() == 1000);

    REQUIRE(p1->getName() == "PROGRAM1");
    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 1);

    if (clear)
    {
        mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/false);
        mpc.getSampler()->deleteAllSamples();
    }

    disk->moveBack();
    disk->initFiles();
    disk->moveForward("program2");
    disk->initFiles();

    auto pgmFile2 = disk->getFile("PROGRAM2.PGM");
    p2 = mpc.getSampler()->createNewProgramAddFirstAvailableSlot().lock();

    ProgramLoader::loadProgram(mpc, pgmFile2, p2);
}

TEST_CASE("Load 2 programs in Clear P & S mode", "[load-programs]")
{
    for (int i = 0; i < 2; i++)
    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        std::shared_ptr<sampler::Program> p1;
        std::shared_ptr<sampler::Program> p2;
        doTest(mpc, true, i == 0, p1, p2);
        REQUIRE(mpc.getSampler()->getProgramCount() == 1);
        REQUIRE(p2 == mpc.getSampler()->getProgram(0));
        REQUIRE(p2->getName() == "PROGRAM2");
        REQUIRE(mpc.getSampler()->getSoundCount() == 2);
        REQUIRE(mpc.getSampler()->getSoundName(0) == "sound2");
        REQUIRE(mpc.getSampler()->getSound(0)->getEnd() == 500);
        REQUIRE(mpc.getSampler()->getSoundName(1) == "sound3");
        REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 0);
        REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 1);
    }
}

TEST_CASE("Load 2 programs in Add to P & S mode", "[load-programs]")
{
    for (int i = 0; i < 2; i++)
    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        std::shared_ptr<sampler::Program> p1;
        std::shared_ptr<sampler::Program> p2;

        doTest(mpc, false, i == 0, p1, p2);

        REQUIRE(mpc.getSampler()->getProgramCount() == 2);
        REQUIRE(mpc.getSampler()->getProgram(1) == p2);
        REQUIRE(mpc.getSampler()->getSoundCount() == 3);
        REQUIRE(mpc.getSampler()->getSoundName(0) == "sound1");
        REQUIRE(mpc.getSampler()->getSoundName(1) == "sound2");
        const int expectedSoundEnd = i == 0 ? 500 : 1000;
        REQUIRE(mpc.getSampler()->getSound(1)->getEnd() == expectedSoundEnd);
        REQUIRE(mpc.getSampler()->getSoundName(2) == "sound3");

        REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == 0);
        REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 1);

        REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 1);
        REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 2);
    }
}

void doTestWithMissingSound(mpc::Mpc &mpc,
                            const bool clear,
                            const bool replaceSameSounds,
                            std::shared_ptr<sampler::Program> &p1,
                            std::shared_ptr<sampler::Program> &p2)
{
    mpc.screens->get<LoadAProgramScreen>()->setLoadReplaceSameSound(replaceSameSounds);

    prepareResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    disk->getFile("SOUND1.SND")->del();
    disk->initFiles();

    auto pgmFile1 = disk->getFile("PROGRAM1.PGM");

    mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/false);
    mpc.getSampler()->deleteAllSamples();

    p1 = mpc.getSampler()->createNewProgramAddFirstAvailableSlot().lock();

    std::thread loadThread([&]()
                           {
                               ProgramLoader::loadProgram(mpc, pgmFile1, p1);
                           });

    int counter = 0;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        counter += 10;

        if (counter > 1000)
        {
            break;
        }

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "cant-find-file")
        {
            auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>();
            cantFindFileScreen->function(1);
            break;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    loadThread.join();

    assert(mpc.getLayeredScreen()->getCurrentScreenName() != "cant-find-file");

    REQUIRE(mpc.getSampler()->getProgram(0) == p1);
    REQUIRE(mpc.getSampler()->getProgramCount() == 1);

    REQUIRE(mpc.getSampler()->getSoundCount() == 1);
    REQUIRE(mpc.getSampler()->getSoundName(0) == "sound2");

    REQUIRE(p1->getName() == "PROGRAM1");
    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == -1);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 0);

    if (clear)
    {
        mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/false);
        mpc.getSampler()->deleteAllSamples();
    }

    disk->moveBack();
    disk->initFiles();
    disk->moveForward("program2");
    disk->initFiles();

    auto pgmFile2 = disk->getFile("PROGRAM2.PGM");
    p2 = mpc.getSampler()->createNewProgramAddFirstAvailableSlot().lock();

    ProgramLoader::loadProgram(mpc, pgmFile2, p2);
}

TEST_CASE("Load 2 programs in Add to P & S mode, 1 missing sound", "[load-programs]")
{
    for (int i = 0; i < 2; i++)
    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        std::shared_ptr<sampler::Program> p1;
        std::shared_ptr<sampler::Program> p2;

        doTestWithMissingSound(mpc, false, i == 0, p1, p2);

        REQUIRE(mpc.getSampler()->getProgramCount() == 2);
        REQUIRE(mpc.getSampler()->getProgram(1) == p2);
        REQUIRE(mpc.getSampler()->getSoundCount() == 2);
        REQUIRE(mpc.getSampler()->getSoundName(0) == "sound2");
        REQUIRE(mpc.getSampler()->getSoundName(1) == "sound3");

        REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == -1);
        REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 0);

        REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 0);
        REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 1);
    }
}