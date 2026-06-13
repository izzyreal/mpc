#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllParser.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>

CMRC_DECLARE(mpctest);

namespace {

void prepareAllResources(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();

    for (auto&& entry : fs.iterate_directory("test/AllLoading")) {
        auto file = fs.open("test/AllLoading/" + entry.filename());
        std::vector<char> data(file.begin(), file.end());
        auto newFile = disk->newFile(entry.filename());
        newFile->setFileData(data);
    }

    disk->initFiles();
}

}

TEST_CASE("Kaitai MPC2000 ALL parses and rewrites real 2KXL ALL", "[kaitai-all][real-2kxl]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc2000xl/All/ALL.ALL");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.defaults() != nullptr);
    REQUIRE(parsed.defaults()->tempo() == 1162);
    REQUIRE(parsed.defaults()->numerator() == 4);
    REQUIRE(parsed.defaults()->denominator() == 4);
    REQUIRE(parsed.defaults()->buses() != nullptr);
    REQUIRE(parsed.defaults()->devices() != nullptr);
    REQUIRE(parsed.defaults()->buses()->at(0) == mpc2000xl_all_t::BUS_DRUM2);
    REQUIRE(parsed.defaults()->devices()->at(0) == 7);

    REQUIRE(parsed.sequences_metas() != nullptr);
    REQUIRE(parsed.sequences() != nullptr);
    REQUIRE(parsed.sequences_metas()->size() >= 2U);
    REQUIRE(parsed.sequences()->size() >= 2U);
    REQUIRE(parsed.sequences_metas()->at(0)->is_used() == mpc2000xl_all_t::SEQUENCE_IS_USED_TRUE);
    REQUIRE(parsed.sequences_metas()->at(1)->is_used() == mpc2000xl_all_t::SEQUENCE_IS_USED_TRUE);
    std::vector<mpc2000xl_all_t::sequence_body_t*> usedSequenceBodies;
    for (const auto& seq : *parsed.sequences()) {
        if (seq->body() != nullptr) {
            usedSequenceBodies.push_back(seq->body());
        }
    }
    REQUIRE(usedSequenceBodies.size() == 2U);
    REQUIRE(usedSequenceBodies.at(0)->index() == 1);
    REQUIRE(usedSequenceBodies.at(1)->index() == 2);
    REQUIRE(usedSequenceBodies.at(0)->tracks() != nullptr);
    REQUIRE(usedSequenceBodies.at(1)->tracks() != nullptr);
    REQUIRE(usedSequenceBodies.at(0)->tracks()->bus()->at(0) == mpc2000xl_all_t::BUS_DRUM2);
    REQUIRE(usedSequenceBodies.at(1)->tracks()->bus()->at(0) == mpc2000xl_all_t::BUS_DRUM2);
    REQUIRE(usedSequenceBodies.at(0)->tracks()->device()->at(0) == 7);
    REQUIRE(usedSequenceBodies.at(1)->tracks()->device()->at(0) == 7);

    REQUIRE(parsed.songs() != nullptr);
    REQUIRE(parsed.songs()->size() >= 2U);
    REQUIRE(parsed.songs()->at(0)->is_used());
    REQUIRE(parsed.songs()->at(1)->is_used());
    REQUIRE(parsed.songs()->at(0)->steps() != nullptr);
    REQUIRE(parsed.songs()->at(1)->steps() != nullptr);
    REQUIRE(parsed.songs()->at(0)->steps()->at(0)->sequence_index() == 0);
    REQUIRE(parsed.songs()->at(0)->steps()->at(0)->repeat_count() == 1);
    REQUIRE(parsed.songs()->at(0)->steps()->at(1)->sequence_index() == 1);
    REQUIRE(parsed.songs()->at(0)->steps()->at(1)->repeat_count() == 1);
    REQUIRE(parsed.songs()->at(1)->steps()->at(0)->sequence_index() == 1);
    REQUIRE(parsed.songs()->at(1)->steps()->at(0)->repeat_count() == 2);
    REQUIRE(parsed.songs()->at(1)->steps()->at(1)->sequence_index() == 0);
    REQUIRE(parsed.songs()->at(1)->steps()->at(1)->repeat_count() == 3);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai MPC2000 ALL matches handwritten ALL bytes", "[kaitai-all]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    prepareAllResources(mpc);

    auto allFile = mpc.getDisk()->getFile("ShouldLoadSeq21.ALL");
    REQUIRE(allFile);

    mpc::file::all::AllParser handwrittenLoader(mpc, allFile->getBytes());
    mpc::disk::AllLoader::loadEverythingFromAllParser(mpc, handwrittenLoader);
    mpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(mpc.getSequencer()->getSequence(0)->isUsed());
    REQUIRE(mpc.getSequencer()->getSequence(20)->isUsed());

    mpc::file::all::AllParser handwrittenWriter(mpc);
    const auto handwrittenBytes = handwrittenWriter.getBytes();

    std::stringstream parseStream(
        std::string(handwrittenBytes.begin(), handwrittenBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.sequences_metas() != nullptr);
    REQUIRE(parsed.sequences_metas()->at(0)->is_used() != mpc2000xl_all_t::SEQUENCE_IS_USED_FALSE);
    REQUIRE(parsed.sequences_metas()->at(20)->is_used() != mpc2000xl_all_t::SEQUENCE_IS_USED_FALSE);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes.size() == handwrittenBytes.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), handwrittenBytes.begin()));

    mpc::Mpc reparsedMpc;
    mpc::TestMpc::initializeTestMpc(reparsedMpc);
    std::vector<char> kaitaiBytesVec(kaitaiBytes.begin(), kaitaiBytes.end());
    mpc::file::all::AllParser reparsed(reparsedMpc, kaitaiBytesVec);
    mpc::disk::AllLoader::loadEverythingFromAllParser(reparsedMpc, reparsed);
    reparsedMpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(reparsedMpc.getSequencer()->getSequence(0)->isUsed());
    REQUIRE(reparsedMpc.getSequencer()->getSequence(20)->isUsed());
}
