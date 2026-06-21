#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/MidIo.hpp"
#include "file/kaitai/generated/standard_midi_file_with_running_status.h"
#include "file/mid/MidiReader.hpp"
#include "file/mid/MidiWriter.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

CMRC_DECLARE(mpctest);

namespace {

struct ParsedNoteSnapshot {
    int note;
    int velocity;
    int duration;
    int variationType;

    bool operator==(const ParsedNoteSnapshot& other) const
    {
        return note == other.note &&
               velocity == other.velocity &&
               duration == other.duration &&
               variationType == other.variationType;
    }
};

std::vector<ParsedNoteSnapshot> collectParsedNotes(const std::shared_ptr<mpc::sequencer::Track>& track)
{
    std::vector<ParsedNoteSnapshot> result;
    for (const auto& event : track->getEvents()) {
        const auto note = std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(event);
        if (!note) {
            continue;
        }
        result.push_back(ParsedNoteSnapshot{
            note->getNote(),
            note->getVelocity(),
            note->getDuration(),
            note->getVariationType()
        });
    }
    return result;
}

std::shared_ptr<mpc::disk::MpcFile> installMidiResourceFile(
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

std::shared_ptr<mpc::disk::MpcFile> installMidiBytes(
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

void insertNote(
    const std::shared_ptr<mpc::sequencer::Track>& track,
    int tick,
    int note,
    int velocity,
    int duration,
    int variationType,
    int variationValue)
{
    mpc::sequencer::EventData eventData;
    eventData.type = mpc::sequencer::EventType::NoteOn;
    eventData.tick = tick;
    eventData.noteNumber = mpc::NoteNumber(note);
    eventData.velocity = mpc::Velocity(velocity);
    eventData.duration = mpc::Duration(duration);
    eventData.noteVariationType = mpc::NoteVariationType(variationType);
    eventData.noteVariationValue = mpc::NoteVariationValue(variationValue);
    track->acquireAndInsertEvent(eventData);
}

}

TEST_CASE("Kaitai standard MIDI parses and rewrites FRUTZLE", "[kaitai-mid]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    standard_midi_file_with_running_status_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.hdr() != nullptr);
    REQUIRE(parsed.hdr()->num_tracks() > 0);
    REQUIRE(parsed.tracks() != nullptr);
    REQUIRE(parsed.tracks()->size() == parsed.hdr()->num_tracks());

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai standard MIDI matches handwritten MIDI bytes", "[kaitai-mid]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto sequence = sequencer->getSequence(0);
    sequence->init(1);
    sequence->setName("KTAIMID");
    stateManager->drainQueue();

    auto track0 = sequence->getTrack(0);
    track0->setUsedIfCurrentlyUnused();
    track0->setDeviceIndex(2);

    mpc::sequencer::EventData eventData;
    eventData.type = mpc::sequencer::EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = mpc::NoteNumber(37);
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(10);
    track0->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    mpc::file::mid::MidiWriter handwrittenWriter(sequence.get());
    auto handwrittenStream = std::make_shared<std::ostringstream>();
    handwrittenWriter.writeToOStream(handwrittenStream);
    const auto handwrittenBytes = handwrittenStream->str();

    std::stringstream parseStream(
        handwrittenBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    standard_midi_file_with_running_status_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.hdr() != nullptr);
    REQUIRE(parsed.hdr()->num_tracks() >= 2);
    REQUIRE(parsed.tracks() != nullptr);
    REQUIRE(parsed.tracks()->size() == parsed.hdr()->num_tracks());

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes.size() == handwrittenBytes.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), handwrittenBytes.begin()));

    auto reloadSequence = sequencer->getSequence(0);
    reloadSequence->init(1);
    stateManager->drainQueue();

    auto input = std::make_shared<std::istringstream>(kaitaiBytes);
    mpc::file::mid::MidiReader reader(input, reloadSequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    REQUIRE(reloadSequence->getTrack(0)->getDeviceIndex() == 2);
    REQUIRE(reloadSequence->getTrack(0)->getEvents().size() == 1);
    auto note = std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(
        reloadSequence->getTrack(0)->getEvents()[0]
    );
    REQUIRE(note);
    REQUIRE(note->getNote() == 37);
    REQUIRE(note->getVelocity() == 127);
    REQUIRE(note->getDuration() == 10);
}

TEST_CASE("Kaitai standard MIDI roundtrips broad handwritten note semantics", "[kaitai-mid]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto sequence = sequencer->getSequence(0);
    sequence->init(1);
    sequence->setName("MIDBROAD");
    stateManager->drainQueue();

    auto track0 = sequence->getTrack(0);
    track0->setUsedIfCurrentlyUnused();
    track0->setBusType(mpc::sequencer::BusType::DRUM3);
    track0->setDeviceIndex(7);
    insertNote(track0, 0, 37, 65, 13, mpc::NoteVariationTypeTune, 80);
    insertNote(track0, 120, 37, 35, 49, mpc::NoteVariationTypeDecay, 26);
    insertNote(track0, 240, 37, 75, 101, mpc::NoteVariationTypeAttack, 31);
    insertNote(track0, 360, 37, 30, 35, mpc::NoteVariationTypeFilter, 61);

    auto track1 = sequence->getTrack(1);
    track1->setUsedIfCurrentlyUnused();
    track1->setBusType(mpc::sequencer::BusType::MIDI);
    track1->setDeviceIndex(2);
    insertNote(track1, 60, 45, 99, 88, mpc::NoteVariationTypeTune, 64);
    stateManager->drainQueue();

    mpc::file::mid::MidiWriter handwrittenWriter(sequence.get());
    auto handwrittenStream = std::make_shared<std::ostringstream>();
    handwrittenWriter.writeToOStream(handwrittenStream);
    const auto handwrittenBytes = handwrittenStream->str();

    std::stringstream parseStream(
        handwrittenBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    standard_midi_file_with_running_status_t parsed(&parseIo);
    parsed._read();

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes.size() == handwrittenBytes.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), handwrittenBytes.begin()));

    auto reloadSequence = sequencer->getSequence(0);
    reloadSequence->init(1);
    stateManager->drainQueue();

    auto input = std::make_shared<std::istringstream>(kaitaiBytes);
    mpc::file::mid::MidiReader reader(input, reloadSequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    REQUIRE(reloadSequence->getTrack(0)->getBusType() == mpc::sequencer::BusType::DRUM3);
    REQUIRE(reloadSequence->getTrack(0)->getDeviceIndex() == 7);
    REQUIRE(reloadSequence->getTrack(1)->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(reloadSequence->getTrack(1)->getDeviceIndex() == 2);

    const std::vector<ParsedNoteSnapshot> expectedTrack0{
        {37, 65, 13, mpc::NoteVariationTypeTune},
        {37, 35, 49, mpc::NoteVariationTypeDecay},
        {37, 75, 101, mpc::NoteVariationTypeAttack},
        {37, 30, 35, mpc::NoteVariationTypeFilter},
    };
    const std::vector<ParsedNoteSnapshot> expectedTrack1{
        {45, 99, 88, mpc::NoteVariationTypeTune},
    };
    REQUIRE(collectParsedNotes(reloadSequence->getTrack(0)) == expectedTrack0);
    REQUIRE(collectParsedNotes(reloadSequence->getTrack(1)) == expectedTrack1);
}

TEST_CASE("Kaitai standard MIDI production load preserves broad handwritten note semantics", "[kaitai-mid]")
{
    mpc::Mpc sourceMpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(sourceMpc);

    auto sourceSequencer = sourceMpc.getSequencer();
    auto sourceStateManager = sourceSequencer->getStateManager();
    auto sourceSequence = sourceSequencer->getSequence(0);
    sourceSequence->init(1);
    sourceSequence->setName("MIDBROAD");
    sourceStateManager->drainQueue();

    auto track0 = sourceSequence->getTrack(0);
    track0->setUsedIfCurrentlyUnused();
    track0->setBusType(mpc::sequencer::BusType::DRUM3);
    track0->setDeviceIndex(7);
    insertNote(track0, 0, 37, 65, 13, mpc::NoteVariationTypeTune, 80);
    insertNote(track0, 120, 37, 35, 49, mpc::NoteVariationTypeDecay, 26);
    insertNote(track0, 240, 37, 75, 101, mpc::NoteVariationTypeAttack, 31);
    insertNote(track0, 360, 37, 30, 35, mpc::NoteVariationTypeFilter, 61);

    auto track1 = sourceSequence->getTrack(1);
    track1->setUsedIfCurrentlyUnused();
    track1->setBusType(mpc::sequencer::BusType::MIDI);
    track1->setDeviceIndex(2);
    insertNote(track1, 60, 45, 99, 88, mpc::NoteVariationTypeTune, 64);
    sourceStateManager->drainQueue();

    auto output = std::make_shared<std::ostringstream>();
    mpc::file::kaitai::MidIo::save(sourceSequence, output);
    const auto saved = output->str();
    std::vector<char> bytes(saved.begin(), saved.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    auto midiFile = installMidiBytes(mpc, bytes, "BROAD.MID");
    REQUIRE(midiFile);

    const auto sequenceOrError = mpc::file::kaitai::MidIo::load(mpc, midiFile);
    REQUIRE(sequenceOrError.has_value());

    auto sequence = sequenceOrError.value();
    auto stateManager = mpc.getSequencer()->getStateManager();
    stateManager->drainQueue();

    REQUIRE(sequence->getTrack(0)->getBusType() == mpc::sequencer::BusType::DRUM3);
    REQUIRE(sequence->getTrack(0)->getDeviceIndex() == 7);
    REQUIRE(sequence->getTrack(1)->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(sequence->getTrack(1)->getDeviceIndex() == 2);

    const std::vector<ParsedNoteSnapshot> expectedTrack0{
        {37, 65, 13, mpc::NoteVariationTypeTune},
        {37, 35, 49, mpc::NoteVariationTypeDecay},
        {37, 75, 101, mpc::NoteVariationTypeAttack},
        {37, 30, 35, mpc::NoteVariationTypeFilter},
    };
    const std::vector<ParsedNoteSnapshot> expectedTrack1{
        {45, 99, 88, mpc::NoteVariationTypeTune},
    };
    REQUIRE(collectParsedNotes(sequence->getTrack(0)) == expectedTrack0);
    REQUIRE(collectParsedNotes(sequence->getTrack(1)) == expectedTrack1);
}

TEST_CASE("Kaitai standard MIDI parses and rewrites real 2KXL SEQ", "[kaitai-mid][real-2kxl]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc2000xl/Mid/mpc2000xl_seq_variations.MID");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    standard_midi_file_with_running_status_t parsed(&parseIo);
    parsed._read();

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    auto stateManager = mpc.getSequencer()->getStateManager();
    sequence->init(1);
    stateManager->drainQueue();

    auto input = std::make_shared<std::istringstream>(rewrittenBytes);
    mpc::file::mid::MidiReader reader(input, sequence);
    REQUIRE_NOTHROW(reader.parseSequence(mpc));
    stateManager->drainQueue();

    REQUIRE(sequence->getBarCount() == 1);

    int totalEvents = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex) {
        totalEvents += static_cast<int>(sequence->getTrack(trackIndex)->getEvents().size());
    }
    REQUIRE(totalEvents == 7);

    const auto parsedNotes = collectParsedNotes(sequence->getTrack(0));
    REQUIRE(parsedNotes.size() == 7U);

    const std::vector<ParsedNoteSnapshot> expected{
        {0, 65, 13, mpc::NoteVariationTypeTune},
        {0, 35, 49, mpc::NoteVariationTypeDecay},
        {0, 75, 101, mpc::NoteVariationTypeAttack},
        {0, 30, 35, mpc::NoteVariationTypeFilter},
        {0, 22, 89, mpc::NoteVariationTypeFilter},
        {0, 46, 174, mpc::NoteVariationTypeTune},
        {0, 19, 161, mpc::NoteVariationTypeTune},
    };
    REQUIRE(parsedNotes.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        CAPTURE(i);
        CAPTURE(parsedNotes[i].note);
        CHECK(parsedNotes[i].velocity == expected[i].velocity);
        CHECK(parsedNotes[i].duration == expected[i].duration);
        CHECK(parsedNotes[i].variationType == expected[i].variationType);
    }
}

TEST_CASE("Kaitai standard MIDI loads real 2KXL SEQ through production seam", "[kaitai-mid][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto midiFile = installMidiResourceFile(
        mpc,
        "test/RealMpc2000xl/Mid/mpc2000xl_seq_variations.MID",
        "SEQ.MID"
    );
    REQUIRE(midiFile);

    const auto sequenceOrError = mpc::file::kaitai::MidIo::load(mpc, midiFile);
    REQUIRE(sequenceOrError.has_value());

    auto sequence = sequenceOrError.value();
    auto stateManager = mpc.getSequencer()->getStateManager();
    stateManager->drainQueue();

    REQUIRE(sequence == mpc.getSequencer()->getSequence(mpc::TempSequenceIndex));
    REQUIRE(sequence->getBarCount() == 1);

    const auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::DRUM2);
    REQUIRE(track0->getDeviceIndex() == 7);

    int totalEvents = 0;
    for (int trackIndex = 0; trackIndex < 64; ++trackIndex) {
        totalEvents += static_cast<int>(sequence->getTrack(trackIndex)->getEvents().size());
    }
    REQUIRE(totalEvents == 7);

    const auto parsedNotes = collectParsedNotes(track0);
    const std::vector<ParsedNoteSnapshot> expected{
        {37, 65, 13, mpc::NoteVariationTypeTune},
        {37, 35, 49, mpc::NoteVariationTypeDecay},
        {37, 75, 101, mpc::NoteVariationTypeAttack},
        {37, 30, 35, mpc::NoteVariationTypeFilter},
        {37, 22, 89, mpc::NoteVariationTypeFilter},
        {37, 46, 174, mpc::NoteVariationTypeTune},
        {37, 19, 161, mpc::NoteVariationTypeTune},
    };
    REQUIRE(parsedNotes == expected);
}
