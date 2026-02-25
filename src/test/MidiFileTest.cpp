#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"

#include "file/mid/MidiReader.hpp"
#include "file/mid/MidiWriter.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <array>
#include <cmrc/cmrc.hpp>
#include <sstream>
#include <vector>

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::disk;
using namespace mpc::file::mid;

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
        track0->setDeviceIndex(2);

        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = 0;
        eventData.noteNumber = NoteNumber(37);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(10);
        track0->acquireAndInsertEvent(eventData);
        stateManager->drainQueue();

        MidiWriter midiWriter(sequence.get());
        auto ostream = std::make_shared<std::ostringstream>();
        midiWriter.writeToOStream(ostream);

        sequence->init(1);
        stateManager->drainQueue();
        track0->removeEvents();
        stateManager->drainQueue();
        REQUIRE(track0->getEvents().empty());

        auto istream = std::make_shared<std::istringstream>(ostream->str());
        MidiReader midiReader(istream, sequence);
        midiReader.parseSequence(mpc);
        stateManager->drainQueue();

        REQUIRE(sequence->getTrack(0)->getEvents().size() == 1);
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(
            sequence->getTrack(0)->getEvents()[0]));
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(
                    sequence->getTrack(0)->getEvents()[0])
                    ->getNote() == 37);
        REQUIRE(sequence->getTrack(0)->getDeviceIndex() == 2);
        REQUIRE(sequence->getTrack(1)->getDeviceIndex() == 0);
    }
}

TEST_CASE("MidiReader parses FRUTZLE.MID without crashing",
          "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto sequence = mpc.getSequencer()->getSequence(0);
    auto stateManager = mpc.getSequencer()->getStateManager();
    sequence->init(1);
    stateManager->drainQueue();

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::string bytes(file.begin(), file.end());
    auto stream = std::make_shared<std::istringstream>(bytes);

    MidiReader reader(stream, sequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    int totalEvents = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        totalEvents += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }

    REQUIRE(totalEvents > 0);
    REQUIRE(sequence->getBarCount() == 4);
}

TEST_CASE("MidiReader parses FRUTZLE.MID into temp sequence without crashing",
          "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto sequence = mpc.getSequencer()->getSequence(TempSequenceIndex);
    auto stateManager = mpc.getSequencer()->getStateManager();
    sequence->init(1);
    stateManager->drainQueue();

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::string bytes(file.begin(), file.end());
    auto stream = std::make_shared<std::istringstream>(bytes);

    MidiReader reader(stream, sequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    int totalEvents = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        totalEvents += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }

    REQUIRE(totalEvents > 0);
}

TEST_CASE("Busy 8-track midi roundtrip preserves note events",
          "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto sequence = sequencer->getSequence(0);
    auto stateManager = sequencer->getStateManager();

    sequence->init(1);
    stateManager->drainQueue();

    for (int trackIndex = 0; trackIndex < 8; ++trackIndex)
    {
        auto track = sequence->getTrack(trackIndex);
        track->setUsedIfCurrentlyUnused();
        track->setDeviceIndex((trackIndex % 4) + 1);

        for (int eventIndex = 0; eventIndex < 32; ++eventIndex)
        {
            EventData eventData;
            eventData.type = EventType::NoteOn;
            eventData.tick =
                (eventIndex * 11 + trackIndex * 17) %
                static_cast<int>(sequence->getLastTick());
            eventData.noteNumber = NoteNumber(35 + eventIndex);
            eventData.velocity = Velocity(20 + ((trackIndex * 13 +
                                                 eventIndex * 5) %
                                                107));
            eventData.duration = Duration(1 + ((trackIndex * 7 +
                                                eventIndex * 13) %
                                               95));
            eventData.noteVariationType =
                NoteVariationType((trackIndex + eventIndex) % 4);

            switch (static_cast<int>(eventData.noteVariationType))
            {
                case 0:
                    eventData.noteVariationValue =
                        NoteVariationValue((eventIndex * 3) % 125);
                    break;
                case 1:
                case 2:
                    eventData.noteVariationValue =
                        NoteVariationValue((trackIndex * 11 + eventIndex) %
                                           101);
                    break;
                default:
                    eventData.noteVariationValue =
                        NoteVariationValue((trackIndex * 9 + eventIndex * 4) %
                                           101);
                    break;
            }

            track->acquireAndInsertEvent(eventData);
        }
    }

    stateManager->drainQueue();

    std::array<std::vector<NoteSnapshot>, 8> expectedPerTrack;
    std::array<int, 8> expectedDeviceIndices{};
    for (int trackIndex = 0; trackIndex < 8; ++trackIndex)
    {
        auto track = sequence->getTrack(trackIndex);
        expectedPerTrack[trackIndex] = collectNoteSnapshots(track);
        expectedDeviceIndices[trackIndex] = track->getDeviceIndex();
        REQUIRE_FALSE(expectedPerTrack[trackIndex].empty());
    }

    MidiWriter writer(sequence.get());
    auto output = std::make_shared<std::ostringstream>();
    writer.writeToOStream(output);

    sequence->init(1);
    stateManager->drainQueue();

    auto input = std::make_shared<std::istringstream>(output->str());
    MidiReader reader(input, sequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    for (int trackIndex = 0; trackIndex < 8; ++trackIndex)
    {
        auto track = sequence->getTrack(trackIndex);
        const auto actual = collectNoteSnapshots(track);

        REQUIRE(track->getDeviceIndex() == expectedDeviceIndices[trackIndex]);
        REQUIRE(actual.size() == expectedPerTrack[trackIndex].size());

        for (size_t i = 0; i < actual.size(); ++i)
        {
            CAPTURE(trackIndex, i);
            CAPTURE(actual[i].tick, expectedPerTrack[trackIndex][i].tick);
            CAPTURE(actual[i].note, expectedPerTrack[trackIndex][i].note);
            CAPTURE(actual[i].velocity,
                    expectedPerTrack[trackIndex][i].velocity);
            CAPTURE(actual[i].duration,
                    expectedPerTrack[trackIndex][i].duration);
            CAPTURE(actual[i].variationType,
                    expectedPerTrack[trackIndex][i].variationType);
            CAPTURE(actual[i].variationValue,
                    expectedPerTrack[trackIndex][i].variationValue);
            REQUIRE(actual[i] == expectedPerTrack[trackIndex][i]);
        }
    }
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

        auto sequence = mpc.getSequencer()->getSequence(TempSequenceIndex);
        auto stateManager = mpc.getSequencer()->getStateManager();
        sequence->init(1);
        stateManager->drainQueue();

        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open("test/MidiFile/" + fileName);
        std::string bytes(file.begin(), file.end());
        auto stream = std::make_shared<std::istringstream>(bytes);

        MidiReader reader(stream, sequence);
        REQUIRE_NOTHROW(reader.parseSequence(mpc));
        stateManager->drainQueue();

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

    auto sequence = mpc.getSequencer()->getSequence(TempSequenceIndex);
    auto stateManager = mpc.getSequencer()->getStateManager();
    sequence->init(1);
    stateManager->drainQueue();

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
    std::string bytes(file.begin(), file.end());
    auto stream = std::make_shared<std::istringstream>(bytes);

    MidiReader reader(stream, sequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    REQUIRE(sequence->getBarCount() == 16);
}

TEST_CASE("FRUTZLE.MID roundtrip keeps non-empty sequence", "[file][midi-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    auto sequence = mpc.getSequencer()->getSequence(TempSequenceIndex);
    auto stateManager = mpc.getSequencer()->getStateManager();
    sequence->init(1);
    stateManager->drainQueue();

    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::string bytes(file.begin(), file.end());
    auto initialInput = std::make_shared<std::istringstream>(bytes);

    MidiReader initialReader(initialInput, sequence);
    REQUIRE_NOTHROW(initialReader.parseSequence(mpc));
    stateManager->drainQueue();

    int eventsAfterFirstLoad = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        eventsAfterFirstLoad += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }
    REQUIRE(eventsAfterFirstLoad > 0);
    REQUIRE(sequence->getBarCount() == 4);

    MidiWriter writer(sequence.get());
    auto output = std::make_shared<std::ostringstream>();
    writer.writeToOStream(output);

    sequence->init(1);
    stateManager->drainQueue();

    auto secondInput = std::make_shared<std::istringstream>(output->str());
    MidiReader secondReader(secondInput, sequence);
    REQUIRE_NOTHROW(secondReader.parseSequence(mpc));
    stateManager->drainQueue();

    int eventsAfterRoundtrip = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex)
    {
        eventsAfterRoundtrip += static_cast<int>(
            sequence->getTrack(trackIndex)->getEvents().size());
    }

    REQUIRE(eventsAfterRoundtrip > 0);
    REQUIRE(sequence->getBarCount() == 4);
}
