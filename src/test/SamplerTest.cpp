#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "disk/SoundLoader.hpp"

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;

void prepareSamplerResources(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk();

    disk->initRoot();
    disk->initFiles();

    auto fs = cmrc::mpctest::get_filesystem();

    for (auto &&entry: fs.iterate_directory("test/Sampler"))
    {
        auto file = fs.open("test/Sampler/" + entry.filename());
        char *data = (char *) std::string_view(file.begin(), file.end() - file.begin()).data();
        auto newFile = disk->newFile(entry.filename());
        std::vector<char> dataVec(data, data + file.size());
        newFile->setFileData(dataVec);
    }

    disk->initFiles();
}

TEST_CASE("Sort sounds by memory index", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    prepareSamplerResources(mpc);

    auto sampler = mpc.getSampler();

    while(sampler->getSoundSortingTypeName() != "MEMORY")
    {
        sampler->switchToNextSoundSortType();
    }

    REQUIRE(sampler->getSoundSortingTypeName() == "MEMORY");

    const bool shouldBeConverted = false;

    SoundLoaderResult r;
    SoundLoader soundLoader(mpc, false);

    for (int i = 3; i > 0; i--)
    {
        auto f = mpc.getDisk()->getFile("SOUND" + std::to_string(i) + ".SND");
        auto s = sampler->addSound("");
        assert(s != nullptr);
        soundLoader.loadSound(f, r, s, shouldBeConverted);
    }

    REQUIRE(sampler->getSoundCount() == 3);

    auto sortedSounds = sampler->getSounds();

    for (int i = 0; i < 3; i++)
    {
        auto s = sortedSounds[i];
        REQUIRE(s->getName() == "sound" + std::to_string(3 - i));
    }

    mpc.getLayeredScreen()->openScreen("trim");
    auto controls = mpc.getActiveControls();
    auto soundName = [&](){ return controls->findChild<lcdgui::Field>("snd")->getText(); };
    REQUIRE(soundName() == "sound3          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound2          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound1          (ST)");
}

TEST_CASE("Sort sounds by name", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    prepareSamplerResources(mpc);

    auto sampler = mpc.getSampler();

    while(sampler->getSoundSortingTypeName() != "NAME")
    {
        sampler->switchToNextSoundSortType();
    }

    REQUIRE(sampler->getSoundSortingTypeName() == "NAME");

    const bool shouldBeConverted = false;

    SoundLoaderResult r;
    SoundLoader soundLoader(mpc, false);

    for (int i = 3; i > 0; i--)
    {
        auto f = mpc.getDisk()->getFile("SOUND" + std::to_string(i) + ".SND");
        auto s = sampler->addSound("");
        assert(s != nullptr);
        soundLoader.loadSound(f, r, s, shouldBeConverted);
    }

    REQUIRE(sampler->getSoundCount() == 3);

    auto sortedSounds = sampler->getSortedSounds();

    for (int i = 0; i < 3; i++)
    {
        auto s = sortedSounds[i];
        REQUIRE(s.first->getName() == "sound" + std::to_string(i + 1));
    }

    mpc.getLayeredScreen()->openScreen("trim");
    auto controls = mpc.getActiveControls();
    auto soundName = [&](){ return controls->findChild<lcdgui::Field>("snd")->getText(); };
    REQUIRE(soundName() == "sound1          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound2          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound3          (ST)");
}

TEST_CASE("Sort sounds by size", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    prepareSamplerResources(mpc);

    auto sampler = mpc.getSampler();

    while(sampler->getSoundSortingTypeName() != "SIZE")
    {
        sampler->switchToNextSoundSortType();
    }

    REQUIRE(sampler->getSoundSortingTypeName() == "SIZE");

    const bool shouldBeConverted = false;

    SoundLoaderResult r;
    SoundLoader soundLoader(mpc, false);

    for (int i = 3; i > 0; i--)
    {
        auto f = mpc.getDisk()->getFile("SOUND" + std::to_string(i) + ".SND");
        auto s = sampler->addSound("");
        assert(s != nullptr);
        soundLoader.loadSound(f, r, s, shouldBeConverted);
    }

    REQUIRE(sampler->getSoundCount() == 3);

    std::vector<int> expected{ 2, 3, 1 };

    auto sortedSounds = sampler->getSortedSounds();

    for (int i = 0; i < 3; i++)
    {
        auto s = sortedSounds[i];
        REQUIRE(s.first->getName() == "sound" + std::to_string(expected[i]));
    }

    mpc.getLayeredScreen()->openScreen("trim");
    auto controls = mpc.getActiveControls();
    auto soundName = [&](){ return controls->findChild<lcdgui::Field>("snd")->getText(); };
    REQUIRE(soundName() == "sound2          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound3          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound1          (ST)");
}

TEST_CASE("Switch sort and retain correct sound index", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    prepareSamplerResources(mpc);

    auto sampler = mpc.getSampler();

    const bool shouldBeConverted = false;

    SoundLoaderResult r;
    SoundLoader soundLoader(mpc, false);

    for (int i = 3; i > 0; i--)
    {
        auto f = mpc.getDisk()->getFile("SOUND" + std::to_string(i) + ".SND");
        auto s = sampler->addSound("");
        assert(s != nullptr);
        soundLoader.loadSound(f, r, s, shouldBeConverted);
    }

    REQUIRE(sampler->getSoundCount() == 3);

    mpc.getLayeredScreen()->openScreen("trim");
    auto controls = mpc.getActiveControls();
    auto soundName = [&](){ return controls->findChild<lcdgui::Field>("snd")->getText(); };
    REQUIRE(soundName() == "sound3          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound2          (ST)");

    while(sampler->getSoundSortingTypeName() != "SIZE")
    {
        sampler->switchToNextSoundSortType();
    }

    mpc.getLayeredScreen()->openScreen("main");
    mpc.getLayeredScreen()->openScreen("trim");

    REQUIRE(soundName() == "sound2          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound3          (ST)");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound1          (ST)");
}

TEST_CASE("Sort does not corrupt note parameter sound indices", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    prepareSamplerResources(mpc);

    auto sampler = mpc.getSampler();

    const bool shouldBeConverted = false;

    SoundLoaderResult r;
    SoundLoader soundLoader(mpc, false);

    for (int i = 0; i < 3; i++)
    {
        auto f = mpc.getDisk()->getFile("SOUND" + std::to_string(i + 1) + ".SND");
        auto s = sampler->addSound("");
        assert(s != nullptr);
        soundLoader.loadSound(f, r, s, shouldBeConverted);
    }

    mpc.getLayeredScreen()->openScreen("program-assign");
    auto controls = mpc.getActiveControls();
    mpc.getLayeredScreen()->setFocus("snd");

    REQUIRE(sampler->getSoundSortingTypeName() == "MEMORY");

    auto soundName = [&](){ return controls->findChild<lcdgui::Field>("snd")->getText(); };
    REQUIRE(soundName() == "OFF");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound1");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound2");
    controls->turnWheel(1);
    REQUIRE(soundName() == "sound3");

    controls->turnWheel(-1);
    REQUIRE(soundName() == "sound2");

    while(sampler->getSoundSortingTypeName() != "SIZE")
    {
        sampler->switchToNextSoundSortType();
    }

    std::vector<int> expected{ 2, 3, 1 };

    auto sortedSounds = sampler->getSortedSounds();

    for (int i = 0; i < 3; i++)
    {
        REQUIRE(soundName() == "sound" + std::to_string(expected[i]));
        controls->turnWheel(1);
    }
}

TEST_CASE("Delete sound 1", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sampler = mpc.getSampler();
    auto program = sampler->getProgram(0);

    for (int i = 0; i < 10; i++)
    {
        auto s = sampler->addSound("");
        assert(s != nullptr);

        if (i % 2 == 0)
        {
            program->getNoteParameters(i + 35)->setSoundIndex(i);
        }
    }

    REQUIRE(program->getNoteParameters(0 + 35)->getSoundIndex() == 0);
    sampler->deleteSound(0);
    REQUIRE(program->getNoteParameters(0 + 35)->getSoundIndex() == -1);
}

TEST_CASE("Delete sound 2", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sampler = mpc.getSampler();
    auto program = sampler->getProgram(0);

    for (int i = 0; i < 10; i++)
    {
        auto s = sampler->addSound("");
        assert(s != nullptr);

        if (i % 2 == 0)
        {
            program->getNoteParameters(i + 35)->setSoundIndex(i);
        }
    }

    REQUIRE(program->getNoteParameters(0 + 35)->getSoundIndex() == 0);
    sampler->deleteSound(1);
    REQUIRE(program->getNoteParameters(0 + 35)->getSoundIndex() == 0);
}

TEST_CASE("Purge unused sounds", "[sampler]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sampler = mpc.getSampler();
    auto program = sampler->getProgram(0);

    for (int i = 0; i < 10; i++)
    {
        auto s = sampler->addSound("");
        assert(s != nullptr);
        s->setName("sound" + std::to_string(i));

        if (i % 2 == 0)
        {
            program->getNoteParameters(i + 35)->setSoundIndex(i);
        }
    }

    REQUIRE(sampler->getUnusedSampleCount() == 5);

    sampler->purge();

    REQUIRE(sampler->getUnusedSampleCount() == 0);

    auto params = program->getNotesParameters();
    REQUIRE(params[0]->getSoundIndex() == 0);
    REQUIRE(params[1]->getSoundIndex() == -1);
    REQUIRE(params[2]->getSoundIndex() == 1);
    REQUIRE(params[3]->getSoundIndex() == -1);
    REQUIRE(params[4]->getSoundIndex() == 2);
    REQUIRE(params[5]->getSoundIndex() == -1);
    REQUIRE(params[6]->getSoundIndex() == 3);
    REQUIRE(params[7]->getSoundIndex() == -1);
    REQUIRE(params[8]->getSoundIndex() == 4);
    REQUIRE(params[9]->getSoundIndex() == -1);
    REQUIRE(sampler->getSound(0)->getName() == "sound0");
    REQUIRE(sampler->getSound(1)->getName() == "sound2");
    REQUIRE(sampler->getSound(2)->getName() == "sound4");
    REQUIRE(sampler->getSound(3)->getName() == "sound6");
    REQUIRE(sampler->getSound(4)->getName() == "sound8");
}
