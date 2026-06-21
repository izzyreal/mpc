#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllParser.hpp"
#include "file/all/AllSong.hpp"
#include "file/all/AllSequence.hpp"
#include "file/all/AllSequencer.hpp"
#include "file/all/Defaults.hpp"
#include "file/all/Tracks.hpp"
#include "file/kaitai/AllIo.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Track.hpp"

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

std::shared_ptr<mpc::disk::MpcFile> installResourceFile(
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

std::shared_ptr<mpc::disk::MpcFile> installAllBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes,
    const std::string& fileName)
{
    auto disk = mpc.getDisk();
    auto newFile = disk->newFile(fileName);
    auto mutableBytes = bytes;
    newFile->setFileData(mutableBytes);
    disk->initFiles();
    return newFile;
}

std::vector<char> buildBroadMutatedAllBytes()
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

    parsed.defaults()->set_tempo(875);
    parsed.defaults()->set_numerator(7);
    parsed.defaults()->set_denominator(8);

    auto defaultDevices = std::make_unique<std::vector<uint8_t>>(*parsed.defaults()->devices());
    defaultDevices->at(0) = 31;
    parsed.defaults()->set_devices(std::move(defaultDevices));

    auto defaultBuses = std::make_unique<std::vector<mpc2000xl_all_t::bus_t>>(*parsed.defaults()->buses());
    defaultBuses->at(0) = mpc2000xl_all_t::BUS_DRUM4;
    parsed.defaults()->set_buses(std::move(defaultBuses));

    std::vector<mpc2000xl_all_t::sequence_body_t*> usedSequenceBodies;
    for (const auto& sequence : *parsed.sequences()) {
        if (sequence->body() != nullptr) {
            usedSequenceBodies.push_back(sequence->body());
        }
    }

    auto sequence0 = usedSequenceBodies.at(0);
    auto sequence0Devices = std::make_unique<std::vector<uint8_t>>(*sequence0->tracks()->device());
    sequence0Devices->at(0) = 29;
    sequence0->tracks()->set_device(std::move(sequence0Devices));

    auto sequence0Buses = std::make_unique<std::vector<mpc2000xl_all_t::bus_t>>(*sequence0->tracks()->bus());
    sequence0Buses->at(0) = mpc2000xl_all_t::BUS_DRUM3;
    sequence0->tracks()->set_bus(std::move(sequence0Buses));

    auto sequence1 = usedSequenceBodies.at(1);
    auto sequence1Devices = std::make_unique<std::vector<uint8_t>>(*sequence1->tracks()->device());
    sequence1Devices->at(0) = 5;
    sequence1->tracks()->set_device(std::move(sequence1Devices));

    auto sequence1Buses = std::make_unique<std::vector<mpc2000xl_all_t::bus_t>>(*sequence1->tracks()->bus());
    sequence1Buses->at(0) = mpc2000xl_all_t::BUS_MIDI;
    sequence1->tracks()->set_bus(std::move(sequence1Buses));

    parsed.sequencer()->set_active_sequence(1);
    parsed.sequencer()->set_active_track(3);
    parsed.sequencer()->set_master_tempo(875);

    auto song0 = parsed.songs()->at(0).get();
    song0->steps()->at(0)->set_sequence_index(1);
    song0->steps()->at(0)->set_repeat_count(4);
    song0->steps()->at(1)->set_sequence_index(0);
    song0->steps()->at(1)->set_repeat_count(5);
    song0->set_loop_first_step(0);
    song0->set_loop_last_step(1);
    song0->set_is_loop_enabled(true);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewritten = writeStream.str();
    return std::vector<char>(rewritten.begin(), rewritten.end());
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

TEST_CASE("Kaitai MPC2000 ALL loads real 2KXL ALL through production seam", "[kaitai-all][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto allFile = installResourceFile(
        mpc,
        "test/RealMpc2000xl/All/ALL.ALL",
        "ALL.ALL"
    );
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(mpc, allFile.get());
    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequencer = mpc.getSequencer();
    auto userScreen = mpc.screens->get<mpc::lcdgui::ScreenId::UserScreen>();
    REQUIRE(userScreen);

    REQUIRE_THAT(userScreen->getTempo(), Catch::Matchers::WithinAbs(116.2, 0.001));
    REQUIRE(userScreen->getBusType() == mpc::sequencer::BusType::DRUM2);
    REQUIRE(userScreen->getDevice() == 7);
    REQUIRE(userScreen->getTimeSig().numerator == 4);
    REQUIRE(userScreen->getTimeSig().denominator == 4);

    auto sequence0 = sequencer->getSequence(0);
    auto sequence1 = sequencer->getSequence(1);
    REQUIRE(sequence0->isUsed());
    REQUIRE(sequence1->isUsed());
    REQUIRE_THAT(sequence0->getInitialTempo(), Catch::Matchers::WithinAbs(116.2, 0.001));
    REQUIRE_THAT(sequence1->getInitialTempo(), Catch::Matchers::WithinAbs(116.2, 0.001));
    REQUIRE(sequence0->getTrack(0)->getBusType() == mpc::sequencer::BusType::DRUM2);
    REQUIRE(sequence1->getTrack(0)->getBusType() == mpc::sequencer::BusType::DRUM2);
    REQUIRE(sequence0->getTrack(0)->getDeviceIndex() == 7);
    REQUIRE(sequence1->getTrack(0)->getDeviceIndex() == 7);
    REQUIRE(sequence0->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(sequence0->getTimeSignatureFromBarIndex(0).denominator == 4);
    REQUIRE(sequence1->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(sequence1->getTimeSignatureFromBarIndex(0).denominator == 4);

    auto song0 = sequencer->getSong(0);
    auto song1 = sequencer->getSong(1);
    REQUIRE(song0->isUsed());
    REQUIRE(song1->isUsed());
    REQUIRE(song0->getStepCount() == 2);
    REQUIRE(song1->getStepCount() == 2);
    REQUIRE(song0->getStep(mpc::SongStepIndex(0)).sequenceIndex == 0);
    REQUIRE(song0->getStep(mpc::SongStepIndex(0)).repetitionCount == 1);
    REQUIRE(song0->getStep(mpc::SongStepIndex(1)).sequenceIndex == 1);
    REQUIRE(song0->getStep(mpc::SongStepIndex(1)).repetitionCount == 1);
    REQUIRE(song1->getStep(mpc::SongStepIndex(0)).sequenceIndex == 1);
    REQUIRE(song1->getStep(mpc::SongStepIndex(0)).repetitionCount == 2);
    REQUIRE(song1->getStep(mpc::SongStepIndex(1)).sequenceIndex == 0);
    REQUIRE(song1->getStep(mpc::SongStepIndex(1)).repetitionCount == 3);
}

TEST_CASE("AllParser reads mutated default tempo and song repeat count", "[kaitai-all]")
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
    parsed.defaults()->set_tempo(999);
    parsed.songs()->at(1)->steps()->at(1)->set_repeat_count(7);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewritten = writeStream.str();
    std::vector<char> bytes(rewritten.begin(), rewritten.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    mpc::file::all::AllParser handwritten(mpc, bytes);

    REQUIRE(handwritten.getDefaults()->getTempo() == 999);
    REQUIRE(handwritten.getSongs().at(1)->getSteps().at(1).second == 7);
}

TEST_CASE("AllLoader loads mutated default tempo and song repeat count", "[kaitai-all]")
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
    parsed.defaults()->set_tempo(999);
    parsed.songs()->at(1)->steps()->at(1)->set_repeat_count(7);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewritten = writeStream.str();
    std::vector<char> bytes(rewritten.begin(), rewritten.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    auto allFile = installAllBytes(mpc, bytes, "MUTATED.ALL");
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(mpc, allFile.get());
    mpc.getSequencer()->getStateManager()->drainQueue();

    auto userScreen = mpc.screens->get<mpc::lcdgui::ScreenId::UserScreen>();
    auto song1 = mpc.getSequencer()->getSong(1);
    REQUIRE(userScreen);
    REQUIRE_THAT(userScreen->getTempo(), Catch::Matchers::WithinAbs(99.9, 0.001));
    REQUIRE(song1->getStep(mpc::SongStepIndex(1)).repetitionCount == 7);
}

TEST_CASE("AllParser reads broad mutated defaults, track routing, and song loop semantics", "[kaitai-all]")
{
    const auto bytes = buildBroadMutatedAllBytes();

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    mpc::file::all::AllParser handwritten(mpc, bytes);

    auto defaults = handwritten.getDefaults();
    REQUIRE(defaults->getTempo() == 875);
    REQUIRE(defaults->getTimeSigNum() == 7);
    REQUIRE(defaults->getTimeSigDen() == 8);
    REQUIRE(defaults->getBusses().at(0) == 4);
    REQUIRE(defaults->getDevices().at(0) == 31);

    auto sequences = handwritten.getAllSequences();
    REQUIRE(sequences.at(0) != nullptr);
    REQUIRE(sequences.at(1) != nullptr);
    REQUIRE(sequences.at(0)->tracks->getBus(0) == 3);
    REQUIRE(sequences.at(0)->tracks->getDeviceIndex(0) == 29);
    REQUIRE(sequences.at(1)->tracks->getBus(0) == 0);
    REQUIRE(sequences.at(1)->tracks->getDeviceIndex(0) == 5);

    auto sequencer = handwritten.getSequencer();
    REQUIRE(sequencer->sequence == 1);
    REQUIRE(sequencer->track == 3);
    REQUIRE_THAT(sequencer->masterTempo, Catch::Matchers::WithinAbs(87.5, 0.001));

    auto songs = handwritten.getSongs();
    REQUIRE(songs.at(0)->getSteps().at(0).first == 1);
    REQUIRE(songs.at(0)->getSteps().at(0).second == 4);
    REQUIRE(songs.at(0)->getSteps().at(1).first == 0);
    REQUIRE(songs.at(0)->getSteps().at(1).second == 5);
    REQUIRE(songs.at(0)->getLoopFirstStepIndex() == 0);
    REQUIRE(songs.at(0)->getLoopLastStepIndex() == 1);
    REQUIRE(songs.at(0)->isLoopEnabled());
}

TEST_CASE("AllLoader loads broad mutated defaults, track routing, and song loop semantics", "[kaitai-all]")
{
    const auto bytes = buildBroadMutatedAllBytes();

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    auto allFile = installAllBytes(mpc, bytes, "BROAD_MUTATED.ALL");
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(mpc, allFile.get());
    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequencer = mpc.getSequencer();
    auto userScreen = mpc.screens->get<mpc::lcdgui::ScreenId::UserScreen>();
    REQUIRE(userScreen);
    REQUIRE_THAT(userScreen->getTempo(), Catch::Matchers::WithinAbs(87.5, 0.001));
    REQUIRE(userScreen->getBusType() == mpc::sequencer::BusType::DRUM4);
    REQUIRE(userScreen->getDevice() == 31);
    REQUIRE(userScreen->getTimeSig().numerator == 7);
    REQUIRE(userScreen->getTimeSig().denominator == 8);

    REQUIRE(sequencer->getSelectedSequenceIndex() == 1);
    REQUIRE(sequencer->getSelectedTrackIndex() == 3);

    auto sequence0 = sequencer->getSequence(0);
    auto sequence1 = sequencer->getSequence(1);
    REQUIRE(sequence0->getTrack(0)->getBusType() == mpc::sequencer::BusType::DRUM3);
    REQUIRE(sequence0->getTrack(0)->getDeviceIndex() == 29);
    REQUIRE(sequence1->getTrack(0)->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(sequence1->getTrack(0)->getDeviceIndex() == 5);

    auto song0 = sequencer->getSong(0);
    REQUIRE(song0->getStep(mpc::SongStepIndex(0)).sequenceIndex == 1);
    REQUIRE(song0->getStep(mpc::SongStepIndex(0)).repetitionCount == 4);
    REQUIRE(song0->getStep(mpc::SongStepIndex(1)).sequenceIndex == 0);
    REQUIRE(song0->getStep(mpc::SongStepIndex(1)).repetitionCount == 5);
    REQUIRE(song0->getFirstLoopStepIndex() == mpc::SongStepIndex(0));
    REQUIRE(song0->getLastLoopStepIndex() == mpc::SongStepIndex(1));
    REQUIRE(song0->isLoopEnabled());
}
