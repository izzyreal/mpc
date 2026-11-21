#include "catch2/catch_test_macros.hpp"
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_approx.hpp>

#include "TestMpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/ProgramLoader.hpp"
#include "lcdgui/screens/window/LoadAProgramScreen.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"
#include "file/kaitai/PgmIo.hpp"
#include "performance/PerformanceManager.hpp"

#include "TestResourceFs.hpp"
#include <string_view>

#include "engine/EngineHost.hpp"


using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

void prepareResources(Mpc &mpc)
{
    auto disk = mpc.getDisk();

    auto fs = mpc_test_resources::get_filesystem();

    for (auto &&entry : fs.iterate_directory("test/ProgramLoading"))
    {
        assert(entry.is_directory());

        disk->newFolder(entry.filename());
        disk->initFiles();
        disk->moveForward(entry.filename());
        disk->initFiles();

        for (auto &&entry2 :
             fs.iterate_directory("test/ProgramLoading/" + entry.filename()))
        {
            auto file = fs.open("test/ProgramLoading/" + entry.filename() +
                                "/" + entry2.filename());
            char *data = (char *)std::string_view(file.begin(),
                                                  file.end() - file.begin())
                             .data();
            auto newFile = disk->newFile(entry2.filename());
            std::vector dataVec(data, data + file.size());
            newFile->setFileData(dataVec);
        }

        disk->moveBack();
        disk->initFiles();
    }
}

void doTest(Mpc &mpc, const bool clear, const bool replaceSameSounds,
            std::shared_ptr<sampler::Program> &p1,
            std::shared_ptr<sampler::Program> &p2)
{
    mpc.screens->get<ScreenId::LoadAProgramScreen>()->setLoadReplaceSameSound(
        replaceSameSounds);

    prepareResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile1 = disk->getFile("PROGRAM1.PGM");

    mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/false);
    mpc.getSampler()->deleteAllSamples();

    p1 = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)ProgramLoader::loadProgram(mpc, pgmFile1, p1, 0);

    mpc.getEngineHost()->prepareProcessBlock(512);

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

    mpc.getEngineHost()->prepareProcessBlock(512);

    disk->moveBack();
    disk->initFiles();
    disk->moveForward("program2");
    disk->initFiles();

    auto pgmFile2 = disk->getFile("PROGRAM2.PGM");
    p2 = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();

    (void)ProgramLoader::loadProgram(mpc, pgmFile2, p2, 1);
    mpc.getEngineHost()->prepareProcessBlock(512);
}

TEST_CASE("Load 2 programs in Clear P & S mode", "[load-programs]")
{
    for (int i = 0; i < 2; i++)
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
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
        TestMpc::initializeTestMpc(mpc);
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

void doTestWithMissingSound(Mpc &mpc, const bool clear,
                            const bool replaceSameSounds,
                            std::shared_ptr<sampler::Program> &p1,
                            std::shared_ptr<sampler::Program> &p2)
{
    mpc.screens->get<ScreenId::LoadAProgramScreen>()->setLoadReplaceSameSound(
        replaceSameSounds);

    prepareResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    disk->getFile("SOUND1.SND")->del();
    disk->initFiles();

    auto pgmFile1 = disk->getFile("PROGRAM1.PGM");

    mpc.getSampler()->deleteAllPrograms(/*createDefaultProgram=*/false);
    mpc.getSampler()->deleteAllSamples();

    p1 = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();

    std::thread loadThread(
        [&]
        {
            (void)ProgramLoader::loadProgram(mpc, pgmFile1, p1, 0);
        });

    int counter = 0;

    bool cantFindFileScreenHasBeenOpened = false;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        mpc.getLayeredScreen()->timerCallback();
        counter += 10;

        if (counter > 1000)
        {
            break;
        }

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "cant-find-file")
        {
            cantFindFileScreenHasBeenOpened = true;
            auto cantFindFileScreen =
                mpc.screens->get<ScreenId::CantFindFileScreen>();
            cantFindFileScreen->function(1);
            break;
        }
    }

    loadThread.join();

    mpc.getLayeredScreen()->timerCallback();

    REQUIRE(cantFindFileScreenHasBeenOpened);
    assert(mpc.getLayeredScreen()->getCurrentScreenName() != "cant-find-file");
    mpc.getEngineHost()->prepareProcessBlock(512);
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
    p2 = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();

    (void)ProgramLoader::loadProgram(mpc, pgmFile2, p2, 1);
}

TEST_CASE("Load 2 programs in Add to P & S mode, 1 missing sound",
          "[load-programs]")
{
    for (int i = 0; i < 2; i++)
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        std::shared_ptr<sampler::Program> p1;
        std::shared_ptr<sampler::Program> p2;

        doTestWithMissingSound(mpc, false, i == 0, p1, p2);
        mpc.getEngineHost()->prepareProcessBlock(512);

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

TEST_CASE("Program loading assigns case-insensitive sound matches",
          "[load-programs][akai-names]")
{
    const bool wav = GENERATE(false, true);
    // Exercise both newly loaded sounds and both policies for existing sounds.
    const int existingSoundMode = GENERATE(0, 1, 2);
    CAPTURE(wav, existingSoundMode);
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sampler = mpc.getSampler();
    const auto disk = mpc.getDisk();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    const auto source = sampler->addSound();
    source->setName("SnareLongName");
    source->setMono(true);
    source->setSampleData(std::make_shared<std::vector<float>>(32, 0.25f));
    const auto program =
        sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    program->getNoteParameters(35)->setSoundIndex(0);
    mpc.getPerformanceManager().lock()->drainQueue();
    auto programBytes = file::kaitai::PgmIo::saveProgram(*program, sampler);
    disk->newFile("TEST.PGM")->setFileDataChecked(programBytes);

    // WAV uses the filename; SND uses its embedded name. Neither has the
    // casing of the PGM reference in this fixture.
    source->setName("SNARELONGNAME");
    if (wav)
    {
        disk->writeWav(source, "");
    }
    else
    {
        disk->writeSnd(source, "");
    }
    sampler->deleteAllSamples();
    if (existingSoundMode != 0)
    {
        const auto existing = sampler->addSound();
        existing->setName("snarelongname");
        existing->setMono(true);
        existing->setSampleData(std::make_shared<std::vector<float>>(16, 0.5f));
    }
    mpc.getPerformanceManager().lock()->drainQueue();
    mpc.screens->get<ScreenId::LoadAProgramScreen>()->setLoadReplaceSameSound(
        existingSoundMode == 2);
    disk->initFiles();
    REQUIRE(disk->getFile(wav ? "SNARELONGNAME.WAV" : "SNARELONGNAME.SND"));
    const auto result = ProgramLoader::loadProgram(
        mpc, disk->getFile("TEST.PGM"), program, 0);
    REQUIRE(result.has_value());
    mpc.getPerformanceManager().lock()->drainQueue();

    REQUIRE(sampler->getSoundCount() == 1);
    CHECK(program->getNoteParameters(35)->getSoundIndex() == 0);
    CHECK(sampler->getSoundName(0) ==
          (existingSoundMode == 1 ? "snarelongname" : "SNARELONGNAME"));
    const auto expectedFrames = existingSoundMode == 1 ? 16 : 32;
    CHECK(sampler->getSound(0)->getSampleData()->size() == expectedFrames);
    CHECK(sampler->getSound(0)->getSampleData()->front() ==
          Catch::Approx(existingSoundMode == 1 ? 0.5f : 0.25f).margin(1.f / 32768));
}

TEST_CASE("Programs keep apostrophe and underscore samples distinct",
          "[load-programs][akai-names]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sampler = mpc.getSampler();
    const auto disk = mpc.getDisk();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();
    const auto program =
        sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    const std::vector<std::string> names{"A'B", "A_B"};
    for (int i = 0; i < names.size(); ++i)
    {
        const auto sound = sampler->addSound();
        sound->setName(names[i]);
        sound->setMono(true);
        sound->setSampleData(
            std::make_shared<std::vector<float>>(32, (i + 1) * 0.25f));
        program->getNoteParameters(35 + i)->setSoundIndex(i);
        disk->writeSnd(sound, "");
    }
    mpc.getPerformanceManager().lock()->drainQueue();
    auto programBytes = file::kaitai::PgmIo::saveProgram(*program, sampler);
    disk->newFile("TEST.PGM")->setFileDataChecked(programBytes);
    sampler->deleteAllSamples();
    mpc.getPerformanceManager().lock()->drainQueue();
    disk->initFiles();
    for (const auto &name : names)
    {
        REQUIRE(disk->getFile(name + ".SND"));
    }
    REQUIRE(ProgramLoader::loadProgram(mpc, disk->getFile("TEST.PGM"),
                                      program, 0).has_value());
    mpc.getPerformanceManager().lock()->drainQueue();
    REQUIRE(sampler->getSoundCount() == 2);
    for (int i = 0; i < names.size(); ++i)
    {
        CHECK(sampler->getSoundName(i) == names[i]);
        CHECK(program->getNoteParameters(35 + i)->getSoundIndex() == i);
        CHECK(sampler->getSound(i)->getSampleData()->front() == (i + 1) * 0.25f);
    }
}
