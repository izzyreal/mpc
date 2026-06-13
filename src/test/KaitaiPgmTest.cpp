#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/ProgramLoader.hpp"
#include "engine/EngineHost.hpp"
#include "file/kaitai/generated/mpc2000xl_pgm.h"
#include "file/pgmwriter/PgmWriter.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

CMRC_DECLARE(mpctest);

namespace {

void prepareProgramLoadingResources(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();

    for (auto&& entry : fs.iterate_directory("test/ProgramLoading")) {
        disk->newFolder(entry.filename());
        disk->initFiles();
        disk->moveForward(entry.filename());
        disk->initFiles();

        for (auto&& entry2 : fs.iterate_directory("test/ProgramLoading/" + entry.filename())) {
            auto file = fs.open("test/ProgramLoading/" + entry.filename() + "/" + entry2.filename());
            std::vector<char> data(file.begin(), file.end());
            auto newFile = disk->newFile(entry2.filename());
            newFile->setFileData(data);
        }

        disk->moveBack();
        disk->initFiles();
    }
}

std::shared_ptr<mpc::sampler::Program> loadProgram1(mpc::Mpc& mpc)
{
    prepareProgramLoadingResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile = disk->getFile("PROGRAM1.PGM");

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto program = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)mpc::disk::ProgramLoader::loadProgram(mpc, pgmFile, program, 0);
    mpc.getEngineHost()->prepareProcessBlock(512);
    return program;
}

}

TEST_CASE("Kaitai MPC2000 PGM parses and rewrites PROGRAM1", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x07\x04", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).size() == 17U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(0).back() == '\0');
    REQUIRE(parsed.sound_names()->at(1).size() == 17U);
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.sound_names()->at(1).back() == '\0');
    REQUIRE(parsed.name().size() == 17U);
    REQUIRE(parsed.name().substr(0, 16) == "VMPCTESTPROGRAM1");
    REQUIRE(parsed.name().back() == '\0');
    REQUIRE(parsed.note_parameters() != nullptr);
    REQUIRE(parsed.note_parameters()->size() == 64U);
    REQUIRE(parsed.note_parameters()->at(0)->sound_index() == 0);
    REQUIRE(parsed.note_parameters()->at(1)->sound_index() == 1);
    REQUIRE(parsed.pad_to_note_mapping() != nullptr);
    REQUIRE(parsed.pad_to_note_mapping()->size() == 64U);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    REQUIRE(writeStream.str() == originalBytes);
}

TEST_CASE("Kaitai MPC2000 PGM parses and rewrites real 2KXL NEWPGM-A", "[kaitai-pgm][real-2kxl]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc2000xl/Pgm/NEWPGM-A.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x07\x04", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 2) == "ML");
    REQUIRE(parsed.sound_names()->at(0).back() == '\0');
    REQUIRE(parsed.sound_names()->at(1).substr(0, 1) == "M");
    REQUIRE(parsed.sound_names()->at(1).back() == '\0');
    REQUIRE(parsed.name().size() == 17U);
    REQUIRE(parsed.name().substr(0, 8) == "NewPgm-A");
    REQUIRE(parsed.name().back() == '\0');
    REQUIRE(parsed.note_parameters() != nullptr);
    REQUIRE(parsed.note_parameters()->size() == 64U);
    REQUIRE(parsed.note_parameters()->at(0)->sound_index() == 255);
    REQUIRE(parsed.note_parameters()->at(1)->sound_index() == 0);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    REQUIRE(writeStream.str() == originalBytes);
}

TEST_CASE("Kaitai MPC2000 PGM matches handwritten PGM bytes", "[kaitai-pgm]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto program = loadProgram1(mpc);
    REQUIRE(program != nullptr);
    REQUIRE(program->getName() == "PROGRAM1");

    mpc::file::pgmwriter::PgmWriter handwrittenWriter(program.get(), mpc.getSampler());
    const auto handwrittenBytes = handwrittenWriter.get();

    std::stringstream parseStream(
        std::string(handwrittenBytes.begin(), handwrittenBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x07\x04", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.name().substr(0, 8) == "PROGRAM1");
    REQUIRE(parsed.note_parameters() != nullptr);
    REQUIRE(parsed.note_parameters()->at(0)->sound_index() == 0);
    REQUIRE(parsed.note_parameters()->at(1)->sound_index() == 1);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes == std::string(handwrittenBytes.begin(), handwrittenBytes.end()));

    auto disk = mpc.getDisk();
    auto kaitaiFile = disk->newFile("KPGM.PGM");
    std::vector<char> kaitaiBytesVec(kaitaiBytes.begin(), kaitaiBytes.end());
    kaitaiFile->setFileData(kaitaiBytesVec);
    disk->initFiles();

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto reparsedProgram = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)mpc::disk::ProgramLoader::loadProgram(mpc, kaitaiFile, reparsedProgram, 0);
    mpc.getEngineHost()->prepareProcessBlock(512);

    REQUIRE(reparsedProgram != nullptr);
    REQUIRE(reparsedProgram->getName() == "KPGM");
    REQUIRE(sampler->getSoundCount() == 2);
    REQUIRE(sampler->getSoundName(0) == "sound1");
    REQUIRE(sampler->getSoundName(1) == "sound2");
    REQUIRE(reparsedProgram->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(reparsedProgram->getNoteParameters(36)->getSoundIndex() == 1);
}
