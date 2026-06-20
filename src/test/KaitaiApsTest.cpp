#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/ApsLoader.hpp"
#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
#include "file/kaitai/ApsIo.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/kaitai/generated/mpc2000xl_aps.h"
#include "performance/PerformanceManager.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>

CMRC_DECLARE(mpctest);

namespace {

void prepareApsResources(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();

    for (auto&& entry : fs.iterate_directory("test/ApsLoading")) {
        auto file = fs.open("test/ApsLoading/" + entry.filename());
        std::vector<char> data(file.begin(), file.end());
        auto newFile = disk->newFile(entry.filename());
        newFile->setFileData(data);
    }

    disk->initFiles();
}

std::shared_ptr<mpc::disk::MpcFile> installApsResourceFile(
    mpc::Mpc& mpc,
    const std::string& resourcePath,
    const std::string& fileName)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(resourcePath);
    std::vector<char> data(file.begin(), file.end());
    auto newFile = disk->newFile(fileName);
    newFile->setFileData(data);
    disk->initFiles();
    return newFile;
}

void loadAllPgmsAps(mpc::Mpc& mpc)
{
    prepareApsResources(mpc);
    auto apsFile = mpc.getDisk()->getFile("ALL_PGMS.APS");
    constexpr bool headless = true;
    mpc::disk::ApsLoader::load(mpc, apsFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);
}

}

TEST_CASE("Kaitai MPC2000 APS parses and rewrites ALL_PGMS", "[kaitai-aps]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ApsLoading/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x0a\x05", 2));
    REQUIRE(parsed.sound_count() == 3);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 3U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.sound_names()->at(2).substr(0, 6) == "sound3");
    REQUIRE(parsed.aps_programs() != nullptr);
    REQUIRE(parsed.aps_programs()->size() >= 2U);
    REQUIRE(parsed.aps_programs()->at(0)->index() == 0);
    REQUIRE(parsed.aps_programs()->at(0)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(0)->body()->name().substr(0, 8) == "PROGRAM1");
    REQUIRE(parsed.aps_programs()->at(0)->body()->note_parameters()->at(0)->sound_index() == 0);
    REQUIRE(parsed.aps_programs()->at(0)->body()->note_parameters()->at(1)->sound_index() == 1);
    REQUIRE(parsed.aps_programs()->at(1)->index() == 1);
    REQUIRE(parsed.aps_programs()->at(1)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(1)->body()->name().substr(0, 8) == "PROGRAM2");
    REQUIRE(parsed.aps_programs()->at(1)->body()->note_parameters()->at(0)->sound_index() == 1);
    REQUIRE(parsed.aps_programs()->at(1)->body()->note_parameters()->at(1)->sound_index() == 2);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai MPC2000 APS parses and rewrites real 2KXL ALL_PGMS", "[kaitai-aps][real-2kxl]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc2000xl/Aps/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x0a\x05", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 1) == "M");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 2) == "ML");
    REQUIRE(parsed.name().substr(0, 8) == "ALL_PGMS");
    REQUIRE(parsed.aps_programs() != nullptr);
    REQUIRE(parsed.aps_programs()->size() >= 1U);
    REQUIRE(parsed.aps_programs()->at(0)->index() == 0);
    REQUIRE(parsed.aps_programs()->at(0)->body() != nullptr);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai MPC2000 APS matches handwritten APS bytes", "[kaitai-aps]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsAps(mpc);

    REQUIRE(mpc.getSampler()->getProgramCount() == 2);
    REQUIRE(mpc.getSampler()->getSoundCount() == 3);

    mpc::file::aps::ApsParser handwrittenWriter(mpc, "ALL_PGMS");
    const auto handwrittenBytes = handwrittenWriter.getBytes();

    std::stringstream parseStream(
        std::string(handwrittenBytes.begin(), handwrittenBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x0a\x05", 2));
    REQUIRE(parsed.sound_count() == 3);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.sound_names()->at(2).substr(0, 6) == "sound3");
    REQUIRE(parsed.aps_programs() != nullptr);
    REQUIRE(parsed.aps_programs()->at(0)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(0)->body()->name().substr(0, 8) == "PROGRAM1");
    REQUIRE(parsed.aps_programs()->at(1)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(1)->body()->name().substr(0, 8) == "PROGRAM2");

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes.size() == handwrittenBytes.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), handwrittenBytes.begin()));

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto disk = mpc.getDisk();
    auto kaitaiFile = disk->newFile("KAPS.APS");
    std::vector<char> kaitaiBytesVec(kaitaiBytes.begin(), kaitaiBytes.end());
    kaitaiFile->setFileData(kaitaiBytesVec);
    disk->initFiles();

    constexpr bool headless = true;
    mpc::disk::ApsLoader::load(mpc, kaitaiFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);

    auto p1 = sampler->getProgram(0);
    auto p2 = sampler->getProgram(1);
    REQUIRE(sampler->getProgramCount() == 2);
    REQUIRE(p1->getName() == "PROGRAM1");
    REQUIRE(p2->getName() == "PROGRAM2");
    REQUIRE(sampler->getSoundCount() == 3);
    REQUIRE(sampler->getSoundName(0) == "sound1");
    REQUIRE(sampler->getSoundName(1) == "sound2");
    REQUIRE(sampler->getSoundName(2) == "sound3");
    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 2);
}

TEST_CASE("Kaitai MPC2000 APS saves MIDI program change separately from slider control change", "[kaitai-aps]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsAps(mpc);

    auto p1 = mpc.getSampler()->getProgram(0);
    REQUIRE(p1 != nullptr);

    p1->setMidiProgramChange(2);
    mpc.getPerformanceManager().lock()->drainQueue();
    REQUIRE(p1->getMidiProgramChange() == 2);
    p1->getSlider()->setControlChange(0);
    mpc::file::aps::ApsParser handwrittenWriterZero(mpc, "ALL_PGMS");
    const auto handwrittenBytesWithZeroControl = handwrittenWriterZero.getBytes();
    std::stringstream handwrittenParseStreamZero(
        std::string(handwrittenBytesWithZeroControl.begin(), handwrittenBytesWithZeroControl.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream handwrittenParseIoZero(&handwrittenParseStreamZero);
    mpc2000xl_aps_t handwrittenParsedZero(&handwrittenParseIoZero);
    handwrittenParsedZero._read();
    REQUIRE(handwrittenParsedZero.aps_programs()->at(0)->body()->program_change() == 1);

    const auto bytesWithZeroControl =
        mpc::file::kaitai::ApsIo::save(mpc, "ALL_PGMS");

    std::stringstream parseStreamZero(
        std::string(bytesWithZeroControl.begin(), bytesWithZeroControl.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIoZero(&parseStreamZero);
    mpc2000xl_aps_t parsedZero(&parseIoZero);
    parsedZero._read();
    REQUIRE(parsedZero.aps_programs()->at(0)->body()->program_change() == 1);

    p1->getSlider()->setControlChange(17);
    const auto bytesWithNonZeroControl =
        mpc::file::kaitai::ApsIo::save(mpc, "ALL_PGMS");

    REQUIRE(bytesWithNonZeroControl == bytesWithZeroControl);
}

TEST_CASE("Kaitai MPC2000 APS loads real 2KXL ALL_PGMS through production seam", "[kaitai-aps][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto apsFile = installApsResourceFile(
        mpc,
        "test/RealMpc2000xl/Aps/ALL_PGMS.APS",
        "ALL_PGMS.APS"
    );
    REQUIRE(apsFile);

    constexpr bool headless = true;
    mpc::file::kaitai::ApsIo::load(mpc, apsFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);

    auto sampler = mpc.getSampler();
    REQUIRE(sampler->getProgramCount() == 1);
    REQUIRE(sampler->getSoundCount() == 0);

    auto p1 = sampler->getProgram(0);
    REQUIRE(p1 != nullptr);
    REQUIRE(p1->getName() == "NewPgm-A");
    REQUIRE(p1->getMidiProgramChange() == 1);
    REQUIRE(p1->getSlider()->getControlChange() == 0);
    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == -1);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == -1);
}
