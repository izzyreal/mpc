#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"

#include "file/kaitai/MidIo.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <array>
#include <cmrc/cmrc.hpp>
#include <vector>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::disk;
namespace
{
    struct NoteSnapshot
    {
        int tick;
        int note;
        int velocity;
        int duration;
        int variationType;
        int variationValue;

        bool operator==(const NoteSnapshot &other) const
        {
            return tick == other.tick && note == other.note &&
                   velocity == other.velocity && duration == other.duration &&
                   variationType == other.variationType &&
                   variationValue == other.variationValue;
        }
    };

    std::vector<NoteSnapshot> collectNoteSnapshots(
        const std::shared_ptr<Track> &track)
    {
        std::vector<NoteSnapshot> result;
        for (const auto &event : track->getEvents())
        {
            const auto note = std::dynamic_pointer_cast<NoteOnEvent>(event);
            if (!note)
            {
                continue;
            }

            result.push_back(NoteSnapshot{
                note->getTick(),
                note->getNote(),
                note->getVelocity(),
                note->getDuration(),
                note->getVariationType(),
                note->getVariationValue()});
        }
        return result;
    }

    std::shared_ptr<Sequence> loadSequenceWithMidIo(
        Mpc &mpc,
        const std::vector<char> &bytes,
        const std::string &fileName)
    {
        const auto stem = fileName.substr(0, fileName.find_last_of('.'));
        const auto sequenceOrError =
            mpc::file::kaitai::MidIo::loadBytes(mpc, bytes, stem);
        REQUIRE(sequenceOrError.has_value());
        mpc.getSequencer()->getStateManager()->drainQueue();
        return sequenceOrError.value();
    }

    std::vector<char> saveSequenceWithMidIo(
        const std::shared_ptr<Sequence> &sequence)
    {
        return mpc::file::kaitai::MidIo::saveBytes(sequence);
    }
} // namespace

SCENARIO("A MidiFile can be written", "[file]")
{

    GIVEN("An Mpc with a Sequence")
    {

        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        auto sequencer = mpc.getSequencer();
        auto stateManager = sequencer->getStateManager();
        auto sequence = sequencer->getSequence(0);
        sequence->init(1);
        stateManager->drainQueue();
        auto track0 = sequence->getTrack(0);
        auto track1 = sequence->getTrack(1);
        track0->setUsedIfCurrentlyUnused();
        track0->setBusType(BusType::DRUM2);
        track0->setDeviceIndex(7);

        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = 0;
        eventData.noteNumber = NoteNumber(37);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(10);
        track0->acquireAndInsertEvent(eventData);
        stateManager->drainQueue();

        const auto savedBytes = saveSequenceWithMidIo(sequence);

        sequence->init(1);
        stateManager->drainQueue();
        track0->removeEvents();
        stateManager->drainQueue();
        REQUIRE(track0->getEvents().empty());

        auto loaded = loadSequenceWithMidIo(
            mpc,
            savedBytes,
            "ROUNDTRIP.MID");

        REQUIRE(loaded->getTrack(0)->getEvents().size() == 1);
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(
            loaded->getTrack(0)->getEvents()[0]));
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(
                    loaded->getTrack(0)->getEvents()[0])
                    ->getNote() == 37);
        REQUIRE(loaded->getTrack(0)->getBusType() == BusType::DRUM2);
        REQUIRE(loaded->getTrack(0)->getDeviceIndex() == 7);
        REQUIRE(loaded->getTrack(1)->getDeviceIndex() == 0);
    }
}

TEST_CASE("MidIo parses FRUTZLE.MID without crashing",
          "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::vector<char> bytes(file.begin(), file.end());
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "FRUTZLE.MID");

    int totalEvents = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        totalEvents += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }

    REQUIRE(totalEvents > 0);
    REQUIRE(sequence->getBarCount() == 4);
}

TEST_CASE("MidIo parses FRUTZLE.MID into temp sequence without crashing",
          "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::vector<char> bytes(file.begin(), file.end());
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "FRUTZLE.MID");

    int totalEvents = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        totalEvents += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }

    REQUIRE(totalEvents > 0);
}

TEST_CASE("Cubase MID files load with valid bar count and name lengths",
          "[file][midi-file]")
{
    const std::array<std::string, 2> fileNames{
        "HWIF316.MID",
        "HWIF6-0V2.MID"};

    for (const auto &fileName : fileNames)
    {
        CAPTURE(fileName);

        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);

        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open("test/MidiFile/" + fileName);
        std::vector<char> bytes(file.begin(), file.end());
        auto sequence = loadSequenceWithMidIo(mpc, bytes, fileName);

        REQUIRE(sequence->getBarCount() > 0);
        REQUIRE(sequence->getName().size() <=
                Mpc2000XlSpecs::MAX_SEQUENCE_NAME_LENGTH);

        for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
        {
            auto trackName = sequence->getTrack(trackIndex)->getName();
            CAPTURE(trackIndex, trackName);
            REQUIRE(trackName.size() <= Mpc2000XlSpecs::MAX_TRACK_NAME_LENGTH);
        }
    }
}

TEST_CASE("HWIF316.MID loads with 16 bars", "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
    std::vector<char> bytes(file.begin(), file.end());
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "HWIF316.MID");

    REQUIRE(sequence->getBarCount() == 16);
}

TEST_CASE("Real 2KXL SEQ.MID preserves MPC note variation semantics",
          "[file][midi-file][real-2kxl]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/SEQ.MID");
    std::vector<char> bytes(file.begin(), file.end());
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "SEQ.MID");

    REQUIRE(sequence->getBarCount() == 1);

    const auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getBusType() == BusType::DRUM2);
    REQUIRE(track0->getDeviceIndex() == 7);

    const auto actual = collectNoteSnapshots(track0);
    const std::vector<NoteSnapshot> expected{
        {0, 37, 65, 13, 0, 0},
        {24, 37, 35, 49, 1, 0},
        {48, 37, 75, 101, 2, 0},
        {72, 37, 30, 35, 3, 0},
        {96, 37, 22, 89, 3, 0},
        {120, 37, 46, 174, 0, 0},
        {144, 37, 19, 161, 0, 0},
    };

    REQUIRE(actual.size() == expected.size());
    for (size_t i = 0; i < actual.size(); ++i)
    {
        CAPTURE(i);
        CAPTURE(actual[i].tick, expected[i].tick);
        CAPTURE(actual[i].note, expected[i].note);
        CAPTURE(actual[i].velocity, expected[i].velocity);
        CAPTURE(actual[i].duration, expected[i].duration);
        CAPTURE(actual[i].variationType, expected[i].variationType);
        REQUIRE(actual[i].tick == expected[i].tick);
        REQUIRE(actual[i].note == expected[i].note);
        REQUIRE(actual[i].velocity == expected[i].velocity);
        REQUIRE(actual[i].duration == expected[i].duration);
        REQUIRE(actual[i].variationType == expected[i].variationType);
    }
}

TEST_CASE("FRUTZLE.MID roundtrip keeps non-empty sequence", "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::vector<char> bytes(file.begin(), file.end());
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "FRUTZLE.MID");

    int eventsAfterFirstLoad = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        eventsAfterFirstLoad += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }
    REQUIRE(eventsAfterFirstLoad > 0);
    REQUIRE(sequence->getBarCount() == 4);

    auto reloaded = loadSequenceWithMidIo(
        mpc,
        saveSequenceWithMidIo(sequence),
        "FRUTZLE.MID");

    int eventsAfterRoundtrip = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        eventsAfterRoundtrip += static_cast<int>(
            reloaded->getTrack(trackIndex)->getEvents().size());
    }

    REQUIRE(eventsAfterRoundtrip > 0);
    REQUIRE(reloaded->getBarCount() == 4);
}
