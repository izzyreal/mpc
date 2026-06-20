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
