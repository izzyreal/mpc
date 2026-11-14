#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "TestMpc.hpp"
#include "nvram/NvRam.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/AllIo.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "file/kaitai/generated/mpc60_all_v2.h"
#include "file/kaitai/generated/mpc3000_all_v3.h"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/Track.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

CMRC_DECLARE(mpctest);

namespace {

std::string exclusiveRecord(const std::string& payload)
{
    // Tick 12, track 3, event ID F0; length completes the first 8-byte segment.
    std::string bytes("\x0c\x00\x00\x03\xf0", 5);
    for (int shift : {0, 8, 16})
    {
        bytes.push_back(static_cast<char>((payload.size() >> shift) & 0xFF));
    }
    bytes += payload;
    // Nonzero padding and closing bytes must survive, not be regenerated.
    bytes += std::string((8 - payload.size() % 8) % 8, char{0x55});
    bytes += std::string("\x0c\x00\x00\x03\xf8\x12\x34\x56", 8);
    return bytes;
}

template <typename T>
std::string rewriteParsedAllRecord(T& parsed)
{
    std::stringstream output(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream io(&output);
    parsed._set_io(&io);
    parsed._check();
    parsed._write();
    return output.str();
}

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

std::string describeFirstByteDiff(
    const std::vector<char>& lhs,
    const std::vector<char>& rhs)
{
    const auto mismatch = std::mismatch(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    if (mismatch.first == lhs.end() && mismatch.second == rhs.end())
    {
        return "no diff";
    }

    const auto offset = static_cast<size_t>(mismatch.first - lhs.begin());
    std::ostringstream oss;
    oss << "first diff at offset " << offset
        << ": lhs=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << (static_cast<unsigned>(static_cast<unsigned char>(*mismatch.first)))
        << " rhs=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << (static_cast<unsigned>(static_cast<unsigned char>(*mismatch.second)));
    return oss.str();
}

std::vector<int> parseAllSequenceBodyIndexes(const std::vector<char>& bytes)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    std::vector<int> result;
    if (parsed.sequences() == nullptr)
    {
        return result;
    }

    for (const auto& sequence : *parsed.sequences())
    {
        if (sequence->body() != nullptr)
        {
            result.push_back(sequence->body()->index());
        }
        else
        {
            result.push_back(-1);
        }
    }

    return result;
}

std::string describeSequenceBodyIndexes(const std::vector<int>& indexes)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        if (i != 0)
        {
            oss << ", ";
        }
        oss << indexes[i];
    }
    oss << "]";
    return oss.str();
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

TEST_CASE("ALL SysEx lengths preserve payload framing and following events",
          "[kaitai-all][all-sysex]")
{
    const int length = GENERATE(0, 1, 2, 7, 8, 9, 15, 16, 17, 267);
    CAPTURE(length);
    std::string payload;
    for (int i = 0; i < length; ++i)
    {
        payload.push_back(static_cast<char>(i % 128));
    }
    const auto original = exclusiveRecord(payload);
    // A normal note event follows immediately after the closing segment.
    const std::string note("\x60\x00\x00\x02\x3c\x0c\x64\x40", 8);
    std::istringstream input(original + note, std::ios::binary);
    kaitai::kstream io(&input);
    mpc2000xl_all_t::event_t event(&io);
    event._read();
    REQUIRE(event.exclusive());
    CHECK(event.exclusive()->payload_length() == length);
    CHECK(event.exclusive()->bytes() == payload);
    CHECK(io.pos() == original.size());
    mpc2000xl_all_t::event_t next(&io);
    next._read();
    REQUIRE(next.note_event());
    CHECK(next.tick() == 96);
    CHECK(next.track() == 2);
    CHECK(next.id() == 60);
    CHECK(io.is_eof());
    CHECK(rewriteParsedAllRecord(event) == original);
    CHECK(rewriteParsedAllRecord(next) == note);
}

TEST_CASE("ALL truncated SysEx framing fails cleanly", "[kaitai-all][all-sysex]")
{
    const auto original = exclusiveRecord(std::string(17, 'A'));
    // Truncation inside the payload, padding, or closing segment.
    const int cut = GENERATE(8 + 16, 8 + 17 + 6, 8 + 17 + 7 + 7);
    CAPTURE(cut);
    std::istringstream input(original.substr(0, cut), std::ios::binary);
    kaitai::kstream io(&input);
    mpc2000xl_all_t::event_t event(&io);
    CHECK_THROWS(event._read());
}

TEST_CASE("ALL long SysEx leaves the next sequence aligned",
          "[kaitai-all][all-sysex]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    const auto file = fs.open("test/RealMpc2000xl/All/ALL.ALL");
    // Reuse a real 10240-byte sequence header; only event framing is varied.
    const std::string header(file.begin() + 14406, file.begin() + 14406 + 10240);
    const std::string note("\x60\x00\x00\x02\x3c\x0c\x64\x40", 8);
    const std::string terminator(8, static_cast<char>(0xFF));
    const auto first = header + exclusiveRecord(std::string(267, 'A')) +
                       note + terminator;
    auto nextHeader = header;
    nextHeader.replace(0, 16, "AFTER_SYSEX     ");
    nextHeader[18] = 2;
    const auto second = nextHeader + note + terminator;
    std::istringstream input(first + second, std::ios::binary);
    kaitai::kstream io(&input);
    mpc2000xl_all_t::sequence_t sequence(&io);
    sequence._read();
    CHECK(io.pos() == first.size());
    REQUIRE(sequence.body()->events()->size() == 3);
    CHECK(sequence.body()->events()->at(0)->exclusive()->bytes() ==
          std::string(267, 'A'));
    CHECK(sequence.body()->events()->at(1)->id() == 60);
    mpc2000xl_all_t::sequence_t next(&io);
    next._read();
    CHECK(next.name() == "AFTER_SYSEX     ");
    CHECK(next.body()->index() == 2);
    CHECK(io.is_eof());
    CHECK(rewriteParsedAllRecord(sequence) == first);
    CHECK(rewriteParsedAllRecord(next) == second);
}

TEST_CASE("ALL short SysEx and mixer serialization retain legacy bytes",
          "[kaitai-all][all-sysex]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequence = mpc.getSequencer()->getSequence(0);
    sequence->init(0);
    mpc::sequencer::EventData sysEx;
    sysEx.type = mpc::sequencer::EventType::SystemExclusive;
    sysEx.tick = 12;
    sysEx.sysExByteA = 0x12;
    sysEx.sysExByteB = 0x34;
    sequence->getTrack(0)->acquireAndInsertEvent(sysEx);
    mpc::sequencer::EventData mixer;
    mixer.type = mpc::sequencer::EventType::Mixer;
    mixer.tick = 24;
    mixer.mixerParameter = 3;
    mixer.mixerPad = 9;
    mixer.mixerValue = 87;
    sequence->getTrack(0)->acquireAndInsertEvent(mixer);
    mpc.getSequencer()->getStateManager()->drainQueue();
    const auto bytes = mpc::file::kaitai::AllIo::save(mpc);
    std::istringstream input(std::string(bytes.begin(), bytes.end()), std::ios::binary);
    kaitai::kstream io(&input);
    mpc2000xl_all_t parsed(&io);
    parsed._read();
    const auto& events = *parsed.sequences()->at(0)->body()->events();
    REQUIRE(events.size() == 3);
    const std::string closing("\x00\x00\x00\x00\xf8\x00\x00\x00", 8);
    const auto expectedSysEx =
        std::string("\x0c\x00\x00\x00\xf0\x02\x00\x00\x12\x34", 10) +
        std::string(6, '\0') + closing;
    const auto expectedMixer =
        std::string("\x18\x00\x00\x00\xf0\x09\x00\x00", 8) +
        std::string("\xf0\x47\x00\x44\x45\x05\x09\x57\xf7", 9) +
        std::string(7, '\0') + closing;
    CHECK(rewriteParsedAllRecord(*events[0]) == expectedSysEx);
    CHECK(rewriteParsedAllRecord(*events[1]) == expectedMixer);
}

TEST_CASE("ALL ordinary Akai SysEx is not treated as a mixer event",
          "[kaitai-all][all-sysex]")
{
    mpc::Mpc source;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(source);
    const auto sequence = source.getSequencer()->getSequence(0);
    sequence->init(0);
    mpc::sequencer::EventData data;
    data.type = mpc::sequencer::EventType::SystemExclusive;
    data.tick = 12;
    data.sysExByteA = 0xF0;
    data.sysExByteB = 0x47;
    sequence->getTrack(0)->acquireAndInsertEvent(data);
    source.getSequencer()->getStateManager()->drainQueue();
    auto bytes = mpc::file::kaitai::AllIo::save(source);

    // Replace the short message by a nine-byte Akai message whose command
    // differs from the known mixer signature, keeping the existing framing.
    std::istringstream input(std::string(bytes.begin(), bytes.end()), std::ios::binary);
    kaitai::kstream io(&input);
    mpc2000xl_all_t parsed(&io);
    parsed._read();
    auto* exclusive = parsed.sequences()->at(0)->body()->events()->at(0)->exclusive();
    REQUIRE(exclusive);
    exclusive->set_bytes(std::string("\xf0\x47\x00\x44\x46\x01\x00\x64\xf7", 9));
    exclusive->set_payload_length(9);
    exclusive->set_padding(std::string(7, '\0'));
    const auto rewritten = rewriteParsedAllRecord(parsed);
    bytes.assign(rewritten.begin(), rewritten.end());

    mpc::Mpc loaded;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(loaded);
    const auto allFile = installAllBytes(loaded, bytes, "AKAI.ALL");
    mpc::disk::AllLoader::loadEverythingFromFile(loaded, allFile.get());
    loaded.getSequencer()->getStateManager()->drainQueue();
    const auto events = loaded.getSequencer()->getSequence(0)->getTrack(0)->getEvents();
    REQUIRE(events.size() == 1);
    const auto event = std::dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(events[0]);
    REQUIRE(event);
    CHECK(event->getByteA() == 0xF0);
    CHECK(event->getByteB() == 0x47);
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
    REQUIRE(parsed.sequences_metas()->at(0)->is_used());
    REQUIRE(parsed.sequences_metas()->at(1)->is_used());
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

TEST_CASE("Kaitai MPC2000 ALL production save preserves real RESIST.ALL bytes", "[kaitai-all][real-2kxl][resist]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto allFile = installResourceFile(
        mpc,
        "test/RealMpc2000xl/Resist/RESIST.ALL",
        "RESIST.ALL"
    );
    REQUIRE(allFile);

    const auto originalBytes = allFile->getBytes();

    mpc::disk::AllLoader::loadEverythingFromFile(mpc, allFile.get());
    mpc.getSequencer()->getStateManager()->drainQueue();

    const auto savedBytes = mpc::file::kaitai::AllIo::save(mpc);
    const auto originalSequenceBodyIndexes = parseAllSequenceBodyIndexes(originalBytes);
    const auto savedSequenceBodyIndexes = parseAllSequenceBodyIndexes(savedBytes);

    INFO("original size: " << originalBytes.size());
    INFO("saved size: " << savedBytes.size());
    INFO(describeFirstByteDiff(originalBytes, savedBytes));
    INFO("original sequence bodies: " << describeSequenceBodyIndexes(originalSequenceBodyIndexes));
    INFO("saved sequence bodies: " << describeSequenceBodyIndexes(savedSequenceBodyIndexes));
    REQUIRE(savedBytes.size() == originalBytes.size());
    REQUIRE(std::equal(savedBytes.begin(), savedBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai MPC2000 ALL saves and reloads ShouldLoadSeq21 semantics", "[kaitai-all]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    prepareAllResources(mpc);

    auto allFile = mpc.getDisk()->getFile("ShouldLoadSeq21.ALL");
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(mpc, allFile.get());
    mpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(mpc.getSequencer()->getSequence(0)->isUsed());
    REQUIRE(mpc.getSequencer()->getSequence(20)->isUsed());

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();
    std::visit(
        [](auto& binding)
        {
            binding.number = -1;
        },
        footswitchController->bindings.at(0)
    );

    const auto kaitaiBytesVec = mpc::file::kaitai::AllIo::save(mpc);

    std::stringstream parseStream(
        std::string(kaitaiBytesVec.begin(), kaitaiBytesVec.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.sequences_metas() != nullptr);
    REQUIRE(parsed.sequences_metas()->at(0)->is_used());
    REQUIRE(parsed.sequences_metas()->at(20)->is_used());
    REQUIRE(parsed.misc() != nullptr);
    REQUIRE(parsed.misc()->midi_switch() != nullptr);
    REQUIRE(parsed.misc()->midi_switch()->at(0)->controller() == 0xFF);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes.size() == kaitaiBytesVec.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), kaitaiBytesVec.begin()));

    mpc::Mpc reparsedMpc;
    mpc::TestMpc::initializeTestMpc(reparsedMpc);
    auto reparsedFile = installAllBytes(
        reparsedMpc,
        std::vector<char>(kaitaiBytes.begin(), kaitaiBytes.end()),
        "ROUNDTRIP.ALL"
    );
    REQUIRE(reparsedFile);
    mpc::disk::AllLoader::loadEverythingFromFile(reparsedMpc, reparsedFile.get());
    reparsedMpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(reparsedMpc.getSequencer()->getSequence(0)->isUsed());
    REQUIRE(reparsedMpc.getSequencer()->getSequence(20)->isUsed());

    auto reparsedFootswitchController =
        reparsedMpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();
    std::visit(
        [](auto& binding)
        {
            REQUIRE(binding.number == -1);
        },
        reparsedFootswitchController->bindings.at(0)
    );
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

TEST_CASE("Kaitai MPC2000 ALL sequence meta infos expose expected real 2KXL names and usedness", "[kaitai-all][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto allFile = installResourceFile(
        mpc,
        "test/RealMpc2000xl/All/ALL.ALL",
        "ALL.ALL"
    );
    REQUIRE(allFile);

    const auto metaInfos = mpc::disk::AllLoader::loadSequenceMetaInfosFromFile(mpc, allFile.get());
    REQUIRE(metaInfos.size() >= 64U);
    REQUIRE(metaInfos[0].used);
    REQUIRE(metaInfos[0].name == "Sequence01      ");
    REQUIRE(metaInfos[1].used);
    REQUIRE(metaInfos[1].name == "Sequence02      ");
    REQUIRE_FALSE(metaInfos[2].used);
    REQUIRE(metaInfos[2].name == "(Unused)        ");
    REQUIRE_FALSE(metaInfos[63].used);
    REQUIRE(metaInfos[63].name == "(Unused)        ");
}

TEST_CASE("Kaitai MPC2000 ALL loads one real 2KXL sequence with expected defaults", "[kaitai-all][real-2kxl]")
{
    mpc::Mpc kaitaiMpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(kaitaiMpc);
    auto kaitaiFile = installResourceFile(
        kaitaiMpc,
        "test/RealMpc2000xl/All/ALL.ALL",
        "ALL.ALL"
    );
    REQUIRE(kaitaiFile);

    const auto loaded = mpc::disk::AllLoader::loadOneSequenceFromFile(
        kaitaiMpc,
        kaitaiFile.get(),
        mpc::SequenceIndex(1),
        mpc::SequenceIndex(10)
    );
    REQUIRE(loaded);
    kaitaiMpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(loaded->isUsed());
    REQUIRE(loaded->getName() == "Sequence02");
    REQUIRE_THAT(loaded->getInitialTempo(), Catch::Matchers::WithinAbs(116.2, 0.001));
    REQUIRE(loaded->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(loaded->getTimeSignatureFromBarIndex(0).denominator == 4);
    REQUIRE(loaded->getTrack(0)->getBusType() == mpc::sequencer::BusType::DRUM2);
    REQUIRE(loaded->getTrack(0)->getDeviceIndex() == 7);
}

TEST_CASE("Kaitai MPC3000 ALL parser exposes expected embedded sequence and song structure", "[kaitai-all][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/All/M3K_2SEQ1SONG.ALL");
    const std::string bytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        bytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_all_v3_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.sequences()->size() == 2U);
    REQUIRE(parsed.sequences()->at(0)->misc_chunks()->sequence_header()->sequence_name() == "SEQ01           ");
    REQUIRE(parsed.sequences()->at(1)->misc_chunks()->sequence_header()->sequence_name() == "SEQ02           ");
    REQUIRE(parsed.sequences()->at(0)->events()->size() == 5U);
    REQUIRE(parsed.sequences()->at(1)->events()->size() == 9U);

    REQUIRE(parsed.songs()->size() == 2U);

    const auto* songBody = parsed.songs()->at(0)->song_body();
    REQUIRE(songBody != nullptr);
    REQUIRE(parsed.songs()->at(0)->number_of_steps() == 2);
    REQUIRE(songBody->song_number() == 1);
    REQUIRE(songBody->song_name() == "SONG01");
    REQUIRE(songBody->end_status() == mpc3000_all_v3_t::song_t::song_body_t::END_STATUS_STOP_AT_END);
    REQUIRE(songBody->loop_back_step_number() == 2);
    REQUIRE(songBody->steps()->size() == 2U);
    REQUIRE(songBody->steps()->at(0)->sequence_number() == 1);
    REQUIRE(songBody->steps()->at(0)->repetition_count() == 2);
    REQUIRE(songBody->steps()->at(1)->sequence_number() == 1);
    REQUIRE(songBody->steps()->at(1)->repetition_count() == 3);

    REQUIRE(parsed.songs()->at(1)->number_of_steps() == 0);
    REQUIRE(parsed.songs()->at(1)->song_body() == nullptr);
}

TEST_CASE("Kaitai MPC3000 ALL sequence meta infos expose expected names and usedness", "[kaitai-all][real-mpc3000]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto allFile = installResourceFile(
        mpc,
        "test/RealMpc3000/All/M3K_2SEQ.ALL",
        "M3K_2SEQ.ALL"
    );
    REQUIRE(allFile);

    const auto metaInfos = mpc::disk::AllLoader::loadSequenceMetaInfosFromFile(mpc, allFile.get());
    REQUIRE(metaInfos.size() == 2U);
    REQUIRE(metaInfos[0].used);
    REQUIRE(metaInfos[0].name == "SEQ01");
    REQUIRE(metaInfos[1].used);
    REQUIRE(metaInfos[1].name == "SEQ02");
}

TEST_CASE("Kaitai MPC3000 ALL empty embedded sequences still expose used sequence meta infos", "[kaitai-all][real-mpc3000]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto allFile = installResourceFile(
        mpc,
        "test/RealMpc3000/All/M3K_EMPTY2.ALL",
        "M3K_EMPTY2.ALL"
    );
    REQUIRE(allFile);

    const auto metaInfos = mpc::disk::AllLoader::loadSequenceMetaInfosFromFile(mpc, allFile.get());
    REQUIRE(metaInfos.size() == 2U);
    REQUIRE(metaInfos[0].used);
    REQUIRE(metaInfos[0].name == "SEQ01");
    REQUIRE(metaInfos[1].used);
    REQUIRE(metaInfos[1].name == "SEQ02");
}

TEST_CASE("Kaitai MPC3000 ALL loads one sequence with expected defaults", "[kaitai-all][real-mpc3000]")
{
    mpc::Mpc kaitaiMpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(kaitaiMpc);
    auto kaitaiFile = installResourceFile(
        kaitaiMpc,
        "test/RealMpc3000/All/M3K_2SEQ.ALL",
        "M3K_2SEQ.ALL"
    );
    REQUIRE(kaitaiFile);

    const auto loaded = mpc::disk::AllLoader::loadOneSequenceFromFile(
        kaitaiMpc,
        kaitaiFile.get(),
        mpc::SequenceIndex(1),
        mpc::SequenceIndex(10)
    );
    REQUIRE(loaded);
    kaitaiMpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(loaded->isUsed());
    REQUIRE(loaded->getName() == "SEQ02");
    REQUIRE_THAT(loaded->getInitialTempo(), Catch::Matchers::WithinAbs(99.0, 0.001));
    REQUIRE(loaded->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(loaded->getTimeSignatureFromBarIndex(0).denominator == 4);
    REQUIRE(loaded->isLoopEnabled());
}

TEST_CASE("Kaitai MPC3000 ALL loads empty embedded sequences without inventing musical events", "[kaitai-all][real-mpc3000]")
{
    mpc::Mpc kaitaiMpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(kaitaiMpc);
    auto kaitaiFile = installResourceFile(
        kaitaiMpc,
        "test/RealMpc3000/All/M3K_EMPTY2.ALL",
        "M3K_EMPTY2.ALL"
    );
    REQUIRE(kaitaiFile);

    const auto loaded = mpc::disk::AllLoader::loadOneSequenceFromFile(
        kaitaiMpc,
        kaitaiFile.get(),
        mpc::SequenceIndex(1),
        mpc::SequenceIndex(10)
    );
    REQUIRE(loaded);
    kaitaiMpc.getSequencer()->getStateManager()->drainQueue();

    REQUIRE(loaded->isUsed());
    REQUIRE(loaded->getName() == "SEQ02");
    REQUIRE(loaded->getLastBarIndex() == 0);
    REQUIRE_THAT(loaded->getInitialTempo(), Catch::Matchers::WithinAbs(99.0, 0.001));
    REQUIRE(loaded->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(loaded->getTimeSignatureFromBarIndex(0).denominator == 4);
    REQUIRE(loaded->isLoopEnabled());
    REQUIRE(loaded->getTrack(0)->getEvents().empty());
}

TEST_CASE("Kaitai MPC3000 ALL loads sequences and songs through the production seam", "[kaitai-all][real-mpc3000]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto allFile = installResourceFile(
        mpc,
        "test/RealMpc3000/All/M3K_2SEQ1SONG.ALL",
        "M3K_2SEQ1SONG.ALL"
    );
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(mpc, allFile.get());
    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequencer = mpc.getSequencer();
    auto sequence0 = sequencer->getSequence(0);
    auto sequence1 = sequencer->getSequence(1);
    REQUIRE(sequence0->isUsed());
    REQUIRE(sequence1->isUsed());
    REQUIRE(sequence0->getName() == "SEQ01");
    REQUIRE(sequence1->getName() == "SEQ02");
    REQUIRE_THAT(sequence0->getInitialTempo(), Catch::Matchers::WithinAbs(99.0, 0.001));
    REQUIRE_THAT(sequence1->getInitialTempo(), Catch::Matchers::WithinAbs(99.0, 0.001));

    auto song0 = sequencer->getSong(0);
    auto song1 = sequencer->getSong(1);
    REQUIRE(song0->isUsed());
    REQUIRE(song0->getName() == "SONG01");
    REQUIRE(song0->getStepCount() == 2);
    REQUIRE(song0->getStep(mpc::SongStepIndex(0)).sequenceIndex == 0);
    REQUIRE(song0->getStep(mpc::SongStepIndex(0)).repetitionCount == 2);
    REQUIRE(song0->getStep(mpc::SongStepIndex(1)).sequenceIndex == 0);
    REQUIRE(song0->getStep(mpc::SongStepIndex(1)).repetitionCount == 3);
    REQUIRE_FALSE(song0->isLoopEnabled());
    REQUIRE_FALSE(song1->isUsed());
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
    std::stringstream reparsedStream(
        rewritten,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream reparsedIo(&reparsedStream);
    mpc2000xl_all_t reparsed(&reparsedIo);
    reparsed._read();

    REQUIRE(reparsed.defaults()->tempo() == 999);
    REQUIRE(reparsed.songs()->at(1)->steps()->at(1)->repeat_count() == 7);
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

    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.defaults()->tempo() == 875);
    REQUIRE(parsed.defaults()->numerator() == 7);
    REQUIRE(parsed.defaults()->denominator() == 8);
    REQUIRE(parsed.defaults()->buses()->at(0) == mpc2000xl_all_t::BUS_DRUM4);
    REQUIRE(parsed.defaults()->devices()->at(0) == 31);

    std::vector<mpc2000xl_all_t::sequence_body_t*> usedSequenceBodies;
    for (const auto& sequence : *parsed.sequences()) {
        if (sequence->body() != nullptr) {
            usedSequenceBodies.push_back(sequence->body());
        }
    }
    REQUIRE(usedSequenceBodies.size() >= 2U);
    REQUIRE(usedSequenceBodies.at(0)->tracks()->bus()->at(0) == mpc2000xl_all_t::BUS_DRUM3);
    REQUIRE(usedSequenceBodies.at(0)->tracks()->device()->at(0) == 29);
    REQUIRE(usedSequenceBodies.at(1)->tracks()->bus()->at(0) == mpc2000xl_all_t::BUS_MIDI);
    REQUIRE(usedSequenceBodies.at(1)->tracks()->device()->at(0) == 5);

    REQUIRE(parsed.sequencer()->active_sequence() == 1);
    REQUIRE(parsed.sequencer()->active_track() == 3);
    REQUIRE(parsed.sequencer()->master_tempo() == 875);

    REQUIRE(parsed.songs()->at(0)->steps()->at(0)->sequence_index() == 1);
    REQUIRE(parsed.songs()->at(0)->steps()->at(0)->repeat_count() == 4);
    REQUIRE(parsed.songs()->at(0)->steps()->at(1)->sequence_index() == 0);
    REQUIRE(parsed.songs()->at(0)->steps()->at(1)->repeat_count() == 5);
    REQUIRE(parsed.songs()->at(0)->loop_first_step() == 0);
    REQUIRE(parsed.songs()->at(0)->loop_last_step() == 1);
    REQUIRE(parsed.songs()->at(0)->is_loop_enabled());
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

TEST_CASE("AllLoader loads representative ALL sequence event types through production seam", "[kaitai-all]")
{
    mpc::Mpc sourceMpc;
    mpc::TestMpc::initializeTestMpc(sourceMpc);

    auto sourceSequencer = sourceMpc.getSequencer();
    auto sourceStateManager = sourceSequencer->getStateManager();
    auto sourceSequence = sourceSequencer->getSequence(0);
    sourceSequence->init(1);
    sourceStateManager->drainQueue();

    auto footswitchController =
        sourceMpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();
    for (int i = 0; i < footswitchController->bindings.size(); ++i)
    {
        std::visit(
            [i](auto& binding)
            {
                binding.number = 20 + i;
            },
            footswitchController->bindings[i]
        );
    }

    auto sourceTrack = sourceSequence->getTrack(63);
    auto openNote = sourceTrack->recordNoteEventNonLive(
        0,
        mpc::NoteNumber(60),
        mpc::Velocity(127),
        0
    );
    sourceTrack->finalizeNoteEventNonLive(openNote, mpc::Duration(1600));
    sourceStateManager->drainQueue();
    sourceTrack->getNoteEvents().front()->setVariationType(mpc::NoteVariationTypeFilter);
    sourceStateManager->drainQueue();
    sourceTrack->getNoteEvents().front()->setVariationValue(mpc::NoteVariationValue(20));
    sourceStateManager->drainQueue();

    mpc::sequencer::EventData programChange;
    programChange.type = mpc::sequencer::EventType::ProgramChange;
    programChange.tick = 96;
    programChange.programChangeProgramIndex = mpc::ProgramIndex(7);
    sourceTrack->acquireAndInsertEvent(programChange);

    mpc::sequencer::EventData controlChange;
    controlChange.type = mpc::sequencer::EventType::ControlChange;
    controlChange.tick = 120;
    controlChange.controllerNumber = 12;
    controlChange.controllerValue = 34;
    sourceTrack->acquireAndInsertEvent(controlChange);

    mpc::sequencer::EventData pitchBend;
    pitchBend.type = mpc::sequencer::EventType::PitchBend;
    pitchBend.tick = 144;
    pitchBend.amount = -1234;
    sourceTrack->acquireAndInsertEvent(pitchBend);

    mpc::sequencer::EventData polyPressure;
    polyPressure.type = mpc::sequencer::EventType::PolyPressure;
    polyPressure.tick = 168;
    polyPressure.noteNumber = mpc::NoteNumber(61);
    polyPressure.amount = 45;
    sourceTrack->acquireAndInsertEvent(polyPressure);

    mpc::sequencer::EventData channelPressure;
    channelPressure.type = mpc::sequencer::EventType::ChannelPressure;
    channelPressure.tick = 192;
    channelPressure.amount = 55;
    sourceTrack->acquireAndInsertEvent(channelPressure);

    mpc::sequencer::EventData systemExclusive;
    systemExclusive.type = mpc::sequencer::EventType::SystemExclusive;
    systemExclusive.tick = 216;
    systemExclusive.sysExByteA = 0x12;
    systemExclusive.sysExByteB = 0x34;
    sourceTrack->acquireAndInsertEvent(systemExclusive);

    mpc::sequencer::EventData mixer;
    mixer.type = mpc::sequencer::EventType::Mixer;
    mixer.tick = 240;
    mixer.mixerParameter = 3;
    mixer.mixerPad = 9;
    mixer.mixerValue = 87;
    sourceTrack->acquireAndInsertEvent(mixer);

    sourceStateManager->drainQueue();

    const auto bytes = mpc::file::kaitai::AllIo::save(sourceMpc);

    mpc::Mpc loadedMpc;
    mpc::TestMpc::initializeTestMpc(loadedMpc);
    auto allFile = installAllBytes(loadedMpc, bytes, "EVENTS.ALL");
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(loadedMpc, allFile.get());
    loadedMpc.getSequencer()->getStateManager()->drainQueue();

    auto loadedSequence = loadedMpc.getSequencer()->getSequence(0);
    REQUIRE(loadedSequence->isUsed());
    auto loadedTrack = loadedSequence->getTrack(63);
    REQUIRE(loadedTrack->getEvents().size() == 8);

    auto noteEvent =
        std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(loadedTrack->getEvent(0));
    REQUIRE(noteEvent);
    REQUIRE(noteEvent->getTick() == 0);
    REQUIRE(noteEvent->getNote() == 60);
    REQUIRE(noteEvent->getVelocity() == 127);
    REQUIRE(noteEvent->getDuration() == 1600);
    REQUIRE(noteEvent->getVariationType() == 3);
    REQUIRE(noteEvent->getVariationValue() == 20);

    auto loadedProgramChange =
        std::dynamic_pointer_cast<mpc::sequencer::ProgramChangeEvent>(loadedTrack->getEvent(1));
    REQUIRE(loadedProgramChange);
    REQUIRE(loadedProgramChange->getTick() == 96);
    REQUIRE(loadedProgramChange->getProgram() == 7);

    auto loadedControlChange =
        std::dynamic_pointer_cast<mpc::sequencer::ControlChangeEvent>(loadedTrack->getEvent(2));
    REQUIRE(loadedControlChange);
    REQUIRE(loadedControlChange->getTick() == 120);
    REQUIRE(loadedControlChange->getController() == 12);
    REQUIRE(loadedControlChange->getAmount() == 34);

    auto loadedPitchBend =
        std::dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(loadedTrack->getEvent(3));
    REQUIRE(loadedPitchBend);
    REQUIRE(loadedPitchBend->getTick() == 144);
    REQUIRE(loadedPitchBend->getAmount() == -1234);

    auto loadedPolyPressure =
        std::dynamic_pointer_cast<mpc::sequencer::PolyPressureEvent>(loadedTrack->getEvent(4));
    REQUIRE(loadedPolyPressure);
    REQUIRE(loadedPolyPressure->getTick() == 168);
    REQUIRE(loadedPolyPressure->getNote() == 61);
    REQUIRE(loadedPolyPressure->getAmount() == 45);

    auto loadedChannelPressure =
        std::dynamic_pointer_cast<mpc::sequencer::ChannelPressureEvent>(loadedTrack->getEvent(5));
    REQUIRE(loadedChannelPressure);
    REQUIRE(loadedChannelPressure->getTick() == 192);
    REQUIRE(loadedChannelPressure->getAmount() == 55);

    auto loadedSystemExclusive =
        std::dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(loadedTrack->getEvent(6));
    REQUIRE(loadedSystemExclusive);
    REQUIRE(loadedSystemExclusive->getTick() == 216);
    REQUIRE(loadedSystemExclusive->getByteA() == 0x12);
    REQUIRE(loadedSystemExclusive->getByteB() == 0x34);

    auto loadedMixer =
        std::dynamic_pointer_cast<mpc::sequencer::MixerEvent>(loadedTrack->getEvent(7));
    REQUIRE(loadedMixer);
    REQUIRE(loadedMixer->getTick() == 240);
    REQUIRE(loadedMixer->getParameter() == 3);
    REQUIRE(loadedMixer->getPad() == 9);
    REQUIRE(loadedMixer->getValue() == 87);
}

TEST_CASE("Kaitai MPC2000 ALL preserves all note variation types through production seam", "[kaitai-all]")
{
    mpc::Mpc sourceMpc;
    mpc::TestMpc::initializeTestMpc(sourceMpc);

    auto sourceSequencer = sourceMpc.getSequencer();
    auto sourceStateManager = sourceSequencer->getStateManager();
    auto sourceSequence = sourceSequencer->getSequence(0);
    sourceSequence->init(0);
    sourceStateManager->drainQueue();

    auto sourceTrack = sourceSequence->getTrack(0);

    const std::array<std::pair<mpc::NoteVariationType, int>, 4> noteVariations{{
        {mpc::NoteVariationTypeTune, 11},
        {mpc::NoteVariationTypeDecay, 22},
        {mpc::NoteVariationTypeAttack, 33},
        {mpc::NoteVariationTypeFilter, 44},
    }};

    int tick = 0;
    int noteNumber = 60;
    for (const auto& [variationType, variationValue] : noteVariations)
    {
        auto noteEvent = sourceTrack->recordNoteEventNonLive(
            tick,
            mpc::NoteNumber(noteNumber),
            mpc::Velocity(100),
            0
        );
        sourceTrack->finalizeNoteEventNonLive(noteEvent, mpc::Duration(24));
        sourceStateManager->drainQueue();

        auto insertedNote = sourceTrack->getNoteEvents().back();
        insertedNote->setVariationType(variationType);
        sourceStateManager->drainQueue();
        insertedNote = sourceTrack->getNoteEvents().back();
        insertedNote->setVariationValue(mpc::NoteVariationValue(variationValue));
        sourceStateManager->drainQueue();

        tick += 24;
        noteNumber += 1;
    }

    REQUIRE(sourceTrack->getEvents().size() == noteVariations.size());
    for (size_t i = 0; i < noteVariations.size(); ++i)
    {
        const auto& [expectedVariationType, expectedVariationValue] = noteVariations[i];
        auto sourceNote =
            std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(sourceTrack->getEvent(static_cast<int>(i)));
        REQUIRE(sourceNote);
        REQUIRE(sourceNote->getVariationType() == expectedVariationType);
        REQUIRE(sourceNote->getVariationValue() == expectedVariationValue);
    }

    const auto bytes = mpc::file::kaitai::AllIo::save(sourceMpc);

    mpc::Mpc loadedMpc;
    mpc::TestMpc::initializeTestMpc(loadedMpc);
    auto allFile = installAllBytes(loadedMpc, bytes, "VARIANTS.ALL");
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(loadedMpc, allFile.get());
    loadedMpc.getSequencer()->getStateManager()->drainQueue();

    auto loadedTrack = loadedMpc.getSequencer()->getSequence(0)->getTrack(0);
    REQUIRE(loadedTrack->getEvents().size() == noteVariations.size());

    for (size_t i = 0; i < noteVariations.size(); ++i)
    {
        const auto& [expectedVariationType, expectedVariationValue] = noteVariations[i];
        auto loadedNote =
            std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(loadedTrack->getEvent(static_cast<int>(i)));
        REQUIRE(loadedNote);
        REQUIRE(loadedNote->getTick() == static_cast<int>(i) * 24);
        REQUIRE(loadedNote->getNote() == 60 + static_cast<int>(i));
        REQUIRE(loadedNote->getDuration() == 24);
        REQUIRE(loadedNote->getVariationType() == expectedVariationType);
        REQUIRE(loadedNote->getVariationValue() == expectedVariationValue);
    }
}

TEST_CASE("Kaitai MPC2000 ALL preserves minimum and maximum pitch bend through production seam", "[kaitai-all]")
{
    mpc::Mpc sourceMpc;
    mpc::TestMpc::initializeTestMpc(sourceMpc);

    auto sourceSequencer = sourceMpc.getSequencer();
    auto sourceStateManager = sourceSequencer->getStateManager();
    auto sourceSequence = sourceSequencer->getSequence(0);
    sourceSequence->init(0);
    sourceStateManager->drainQueue();

    auto sourceTrack = sourceSequence->getTrack(0);

    mpc::sequencer::EventData minPitchBend;
    minPitchBend.type = mpc::sequencer::EventType::PitchBend;
    minPitchBend.tick = 0;
    minPitchBend.amount = -8192;
    sourceTrack->acquireAndInsertEvent(minPitchBend);

    mpc::sequencer::EventData maxPitchBend;
    maxPitchBend.type = mpc::sequencer::EventType::PitchBend;
    maxPitchBend.tick = 24;
    maxPitchBend.amount = 8191;
    sourceTrack->acquireAndInsertEvent(maxPitchBend);

    sourceStateManager->drainQueue();

    const auto bytes = mpc::file::kaitai::AllIo::save(sourceMpc);

    mpc::Mpc loadedMpc;
    mpc::TestMpc::initializeTestMpc(loadedMpc);
    auto allFile = installAllBytes(loadedMpc, bytes, "PITCHMAX.ALL");
    REQUIRE(allFile);

    mpc::disk::AllLoader::loadEverythingFromFile(loadedMpc, allFile.get());
    loadedMpc.getSequencer()->getStateManager()->drainQueue();

    auto loadedTrack = loadedMpc.getSequencer()->getSequence(0)->getTrack(0);
    REQUIRE(loadedTrack->getEvents().size() == 2);

    auto loadedMin =
        std::dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(loadedTrack->getEvent(0));
    REQUIRE(loadedMin);
    REQUIRE(loadedMin->getTick() == 0);
    REQUIRE(loadedMin->getAmount() == -8192);

    auto loadedMax =
        std::dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(loadedTrack->getEvent(1));
    REQUIRE(loadedMax);
    REQUIRE(loadedMax->getTick() == 24);
    REQUIRE(loadedMax->getAmount() == 8191);
}

TEST_CASE("User defaults restore unsigned program and velocity values",
          "[defaults][byte-decode]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const auto user = mpc.screens->get<mpc::lcdgui::ScreenId::UserScreen>();
    const auto screen = mpc.getLayeredScreen();
    screen->openScreen("user");
    for (const auto program : {127, 128})
    {
        for (const auto velocity : {127, 128, 200})
        {
            CAPTURE(program, velocity);
            REQUIRE(screen->setFocus("pgm"));
            user->turnWheel(program - user->getPgm());
            REQUIRE(screen->setFocus("velo"));
            user->turnWheel(velocity - user->getVelo());
            mpc::nvram::NvRam::saveUserScreenValues(mpc);
            REQUIRE(screen->setFocus("pgm"));
            user->turnWheel(-user->getPgm());
            REQUIRE(screen->setFocus("velo"));
            user->turnWheel(1 - user->getVelo());
            mpc::nvram::NvRam::loadUserScreenValues(mpc);
            REQUIRE(user->getPgm() == program);
            REQUIRE(user->getVelo() == velocity);
        }
    }
}
