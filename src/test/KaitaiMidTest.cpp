#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/MidIo.hpp"
#include "file/kaitai/generated/standard_midi_file_with_running_status.h"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <cmath>
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

template<typename T>
std::shared_ptr<T> findEventOfType(const std::shared_ptr<mpc::sequencer::Track>& track)
{
    for (const auto& event : track->getEvents()) {
        if (const auto cast = std::dynamic_pointer_cast<T>(event)) {
            return cast;
        }
    }
    return {};
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

std::vector<char> saveSequenceWithMidIo(
    const std::shared_ptr<mpc::sequencer::Sequence>& sequence)
{
    return mpc::file::kaitai::MidIo::saveBytes(sequence);
}

std::shared_ptr<mpc::sequencer::Sequence> loadSequenceWithMidIo(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes,
    const std::string& fileName)
{
    const auto stem = fileName.substr(0, fileName.find_last_of('.'));
    const auto sequenceOrError = mpc::file::kaitai::MidIo::loadBytes(mpc, bytes, stem);
    REQUIRE(sequenceOrError.has_value());
    auto stateManager = mpc.getSequencer()->getStateManager();
    stateManager->drainQueue();
    return sequenceOrError.value();
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

TEST_CASE("Kaitai standard MIDI saves and loads explicit sequence semantics", "[kaitai-mid]")
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

    const auto savedBytes = saveSequenceWithMidIo(sequence);

    std::stringstream parseStream(
        std::string(savedBytes.begin(), savedBytes.end()),
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
    REQUIRE(kaitaiBytes.size() == savedBytes.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), savedBytes.begin()));

    auto reloadSequence = loadSequenceWithMidIo(
        mpc,
        std::vector<char>(kaitaiBytes.begin(), kaitaiBytes.end()),
        "KTAIMID.MID");

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

TEST_CASE("Kaitai standard MIDI roundtrips broad sequence semantics", "[kaitai-mid]")
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

    const auto savedBytes = saveSequenceWithMidIo(sequence);

    std::stringstream parseStream(
        std::string(savedBytes.begin(), savedBytes.end()),
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
    REQUIRE(kaitaiBytes.size() == savedBytes.size());
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), savedBytes.begin()));

    auto reloadSequence = loadSequenceWithMidIo(
        mpc,
        std::vector<char>(kaitaiBytes.begin(), kaitaiBytes.end()),
        "MIDBROAD.MID");

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

TEST_CASE("Kaitai standard MIDI production load preserves broad sequence semantics", "[kaitai-mid]")
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

TEST_CASE("Kaitai standard MIDI roundtrips MPC2000XL meta and channel events", "[kaitai-mid]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto sequence = sequencer->getSequence(0);
    sequence->init(1);
    sequence->setName("MIDMETA");
    sequence->setLoopEnabled(false);
    sequence->setLastLoopBarIndex(mpc::BarIndex(1));
    sequence->setInitialTempo(98.7);
    sequence->setTempoChangeOn(true);
    sequence->addTempoChangeEvent(96, 1250);
    sequence->setTimeSignature(1, 3, 4);

    auto track = sequence->getTrack(0);
    track->setUsedIfCurrentlyUnused();
    track->setName("CTRLMETA");
    track->setBusType(mpc::sequencer::BusType::MIDI);
    track->setDeviceIndex(5);

    insertNote(track, 0, 57, 101, 24, mpc::NoteVariationTypeTune, 64);

    mpc::sequencer::EventData polyPressure;
    polyPressure.type = mpc::sequencer::EventType::PolyPressure;
    polyPressure.tick = 12;
    polyPressure.noteNumber = mpc::NoteNumber(57);
    polyPressure.amount = 54;
    track->acquireAndInsertEvent(polyPressure);

    mpc::sequencer::EventData controlChange;
    controlChange.type = mpc::sequencer::EventType::ControlChange;
    controlChange.tick = 24;
    controlChange.controllerNumber = 7;
    controlChange.controllerValue = 99;
    track->acquireAndInsertEvent(controlChange);

    mpc::sequencer::EventData programChange;
    programChange.type = mpc::sequencer::EventType::ProgramChange;
    programChange.tick = 36;
    programChange.programChangeProgramIndex = mpc::ProgramIndex(12);
    track->acquireAndInsertEvent(programChange);

    mpc::sequencer::EventData channelPressure;
    channelPressure.type = mpc::sequencer::EventType::ChannelPressure;
    channelPressure.tick = 48;
    channelPressure.amount = 60;
    track->acquireAndInsertEvent(channelPressure);

    mpc::sequencer::EventData mixer;
    mixer.type = mpc::sequencer::EventType::Mixer;
    mixer.tick = 60;
    mixer.mixerParameter = 3;
    mixer.mixerPad = 12;
    mixer.mixerValue = 87;
    track->acquireAndInsertEvent(mixer);

    stateManager->drainQueue();

    auto reloaded = loadSequenceWithMidIo(mpc, saveSequenceWithMidIo(sequence), "MIDMETA.MID");
    auto reloadedTrack = reloaded->getTrack(0);

    REQUIRE(reloaded->getName() == "MIDMETA         ");
    REQUIRE(reloaded->isLoopEnabled() == false);
    REQUIRE(reloaded->getFirstLoopBarIndex() == mpc::BarIndex(0));
    REQUIRE(reloaded->getLastLoopBarIndex() == mpc::EndOfSequence);
    REQUIRE(mpc.getSequencer()->getTransport()->isTempoSourceSequence());
    REQUIRE(std::abs(reloaded->getInitialTempo() - 98.7) < 0.2);
    REQUIRE(reloaded->getNumerator(0) == 4);
    REQUIRE(reloaded->getDenominator(0) == 4);
    REQUIRE(reloaded->getNumerator(1) == 3);
    REQUIRE(reloaded->getDenominator(1) == 4);
    REQUIRE(reloadedTrack->getName() == "CTRLMETA        ");
    REQUIRE(reloadedTrack->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(reloadedTrack->getDeviceIndex() == 5);

    const auto notes = collectParsedNotes(reloadedTrack);
    REQUIRE(notes == std::vector<ParsedNoteSnapshot>{
        {57, 101, 24, mpc::NoteVariationTypeTune}
    });

    const auto reloadedPolyPressure =
        findEventOfType<mpc::sequencer::PolyPressureEvent>(reloadedTrack);
    REQUIRE(reloadedPolyPressure);
    REQUIRE(reloadedPolyPressure->getNote() == 57);
    REQUIRE(reloadedPolyPressure->getAmount() == 54);

    const auto reloadedControlChange =
        findEventOfType<mpc::sequencer::ControlChangeEvent>(reloadedTrack);
    REQUIRE(reloadedControlChange);
    REQUIRE(reloadedControlChange->getController() == 7);
    REQUIRE(reloadedControlChange->getAmount() == 99);

    const auto reloadedProgramChange =
        findEventOfType<mpc::sequencer::ProgramChangeEvent>(reloadedTrack);
    REQUIRE(reloadedProgramChange);
    REQUIRE(reloadedProgramChange->getProgram() == 12);

    const auto reloadedChannelPressure =
        findEventOfType<mpc::sequencer::ChannelPressureEvent>(reloadedTrack);
    REQUIRE(reloadedChannelPressure);
    REQUIRE(reloadedChannelPressure->getAmount() == 60);

    const auto reloadedMixer =
        findEventOfType<mpc::sequencer::MixerEvent>(reloadedTrack);
    REQUIRE(reloadedMixer);
    REQUIRE(reloadedMixer->getParameter() == 3);
    REQUIRE(reloadedMixer->getPad() == 12);
    REQUIRE(reloadedMixer->getValue() == 87);

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

    auto midiFile = installMidiBytes(
        mpc,
        std::vector<char>(rewrittenBytes.begin(), rewrittenBytes.end()),
        "SEQ.MID");
    REQUIRE(midiFile);
    const auto sequenceOrError = mpc::file::kaitai::MidIo::load(mpc, midiFile);
    REQUIRE(sequenceOrError.has_value());
    sequence = sequenceOrError.value();
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
