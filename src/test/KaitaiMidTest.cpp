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
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

CMRC_DECLARE(mpctest);

namespace {

using midi_t = standard_midi_file_with_running_status_t;
using meta_type_t = midi_t::meta_event_body_t::meta_type_enum_t;

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

struct DetailedParsedNoteSnapshot {
    int tick;
    int note;
    int velocity;
    int duration;
    int variationType;
    int variationValue;

    bool operator==(const DetailedParsedNoteSnapshot& other) const
    {
        return tick == other.tick &&
               note == other.note &&
               velocity == other.velocity &&
               duration == other.duration &&
               variationType == other.variationType &&
               variationValue == other.variationValue;
    }
};

struct MidiTimelineEventSnapshot {
    int tick;
    std::string kind;
    int a;
    int b;

    bool operator==(const MidiTimelineEventSnapshot& other) const
    {
        return tick == other.tick &&
               kind == other.kind &&
               a == other.a &&
               b == other.b;
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

std::string asciiBody(const std::string& body)
{
    return body;
}

double bpmFromTempoBody(const std::string& body)
{
    if (body.size() < 3)
    {
        return 120.0;
    }

    const auto mpqn =
        (static_cast<unsigned char>(body[0]) << 16) |
        (static_cast<unsigned char>(body[1]) << 8) |
        static_cast<unsigned char>(body[2]);
    return mpqn == 0 ? 120.0 : 60000000.0 / static_cast<double>(mpqn);
}

int denominatorFromTimeSignatureBody(const std::string& body)
{
    return body.size() < 2 ? mpc::DefaultTimeSigDenominator
                           : 1 << static_cast<unsigned char>(body[1]);
}

std::vector<DetailedParsedNoteSnapshot> collectDetailedParsedNotes(
    const std::shared_ptr<mpc::sequencer::Track>& track)
{
    std::vector<DetailedParsedNoteSnapshot> result;
    for (const auto& event : track->getEvents()) {
        const auto note = std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(event);
        if (!note) {
            continue;
        }
        result.push_back(DetailedParsedNoteSnapshot{
            note->getTick(),
            note->getNote(),
            note->getVelocity(),
            note->getDuration(),
            note->getVariationType(),
            note->getVariationValue()
        });
    }
    return result;
}

std::vector<MidiTimelineEventSnapshot> collectKaitaiTimeline(const std::vector<char>& bytes, size_t trackIndex)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    midi_t parsed(&parseIo);
    parsed._read();

    REQUIRE(trackIndex < parsed.tracks()->size());
    const auto& track = parsed.tracks()->at(trackIndex);

    std::vector<MidiTimelineEventSnapshot> result;
    int tick = 0;
    for (const auto& eventPtr : *track->events()->event())
    {
        tick += eventPtr->v_time()->value();
        if (const auto* meta = eventPtr->meta_event_body(); meta != nullptr)
        {
            switch (meta->meta_type())
            {
                case meta_type_t::META_TYPE_ENUM_SEQUENCE_TRACK_NAME:
                    result.push_back({tick, "track_name", static_cast<int>(asciiBody(meta->body()).size()), 0});
                    break;
                case meta_type_t::META_TYPE_ENUM_TEXT_EVENT:
                    result.push_back({tick, "text", static_cast<int>(asciiBody(meta->body()).size()), 0});
                    break;
                case meta_type_t::META_TYPE_ENUM_TEMPO:
                    result.push_back({tick, "tempo", static_cast<int>(std::lround(bpmFromTempoBody(meta->body()) * 10.0)), 0});
                    break;
                case meta_type_t::META_TYPE_ENUM_TIME_SIGNATURE:
                    result.push_back({tick, "time_signature", meta->body().empty() ? mpc::DefaultTimeSigNumerator : static_cast<unsigned char>(meta->body()[0]), denominatorFromTimeSignatureBody(meta->body())});
                    break;
                case meta_type_t::META_TYPE_ENUM_END_OF_TRACK:
                    break;
                default:
                    result.push_back({tick, "meta_other", static_cast<int>(meta->meta_type()), static_cast<int>(meta->body().size())});
                    break;
            }
            continue;
        }

        switch (eventPtr->event_type())
        {
            case 0x80:
            {
                const auto* noteOff = dynamic_cast<midi_t::note_off_event_t*>(eventPtr->event_body());
                result.push_back({tick, "note_off", noteOff->note(), noteOff->velocity()});
                break;
            }
            case 0x90:
            {
                const auto* noteOn = dynamic_cast<midi_t::note_on_event_t*>(eventPtr->event_body());
                result.push_back({tick, "note_on", noteOn->note(), noteOn->velocity()});
                break;
            }
            case 0xA0:
            {
                const auto* noteAftertouch = dynamic_cast<midi_t::polyphonic_pressure_event_t*>(eventPtr->event_body());
                result.push_back({tick, "poly_pressure", noteAftertouch->note(), noteAftertouch->pressure()});
                break;
            }
            case 0xB0:
            {
                const auto* controller = dynamic_cast<midi_t::controller_event_t*>(eventPtr->event_body());
                result.push_back({tick, "control_change", controller->controller(), controller->value()});
                break;
            }
            case 0xC0:
            {
                const auto* programChange = dynamic_cast<midi_t::program_change_event_t*>(eventPtr->event_body());
                result.push_back({tick, "program_change", programChange->program(), 0});
                break;
            }
            case 0xD0:
            {
                const auto* channelPressure = dynamic_cast<midi_t::channel_pressure_event_t*>(eventPtr->event_body());
                result.push_back({tick, "channel_pressure", channelPressure->pressure(), 0});
                break;
            }
            case 0xE0:
            {
                const auto* pitchBend = dynamic_cast<midi_t::pitch_bend_event_t*>(eventPtr->event_body());
                result.push_back({tick, "pitch_bend", pitchBend->b1(), pitchBend->b2()});
                break;
            }
            case 0xF0:
            {
                const auto* sysEx = eventPtr->sysex_body();
                result.push_back({tick, "sysex", static_cast<int>(sysEx->data().size()), sysEx->data().empty() ? -1 : static_cast<unsigned char>(sysEx->data()[0])});
                break;
            }
            default:
                result.push_back({tick, "other", eventPtr->event_type(), 0});
                break;
        }
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

std::shared_ptr<mpc::sequencer::Sequence> loadSequenceWithDirectGenericKaitai(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes,
    const std::string& fileName)
{
    const auto stem = fileName.substr(0, fileName.find_last_of('.'));
    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    sequence->init(0);
    mpc.getSequencer()->getStateManager()->drainQueue();

    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    midi_t parsed(&parseIo);
    parsed._read();

    auto sequencer = mpc.getSequencer();
    auto transport = sequencer->getTransport();

    std::vector<std::pair<int, double>> tempoChanges;
    struct TimeSigChange { int tick; int numerator; int denominator; };
    std::vector<TimeSigChange> timeSignatures;

    int lengthInTicks = 0;
    for (const auto& midiTrack : *parsed.tracks())
    {
        int trackTick = 0;
        for (const auto& eventPtr : *midiTrack->events()->event())
        {
            trackTick += eventPtr->v_time()->value();
        }
        lengthInTicks = std::max(lengthInTicks, trackTick);
    }

    int tick = 0;
    for (const auto& eventPtr : *parsed.tracks()->at(0)->events()->event())
    {
        tick += eventPtr->v_time()->value();
        const auto* meta = eventPtr->meta_event_body();
        if (meta == nullptr)
        {
            continue;
        }

        if (meta->meta_type() == meta_type_t::META_TYPE_ENUM_TEMPO)
        {
            tempoChanges.push_back({tick, bpmFromTempoBody(meta->body())});
        }
        else if (meta->meta_type() == meta_type_t::META_TYPE_ENUM_TIME_SIGNATURE)
        {
            timeSignatures.push_back(
                TimeSigChange{
                    tick,
                    meta->body().empty()
                        ? mpc::DefaultTimeSigNumerator
                        : static_cast<unsigned char>(meta->body()[0]),
                    denominatorFromTimeSignatureBody(meta->body())
                }
            );
        }
    }

    UNSCOPED_INFO("direct-generic file=" << fileName);
    UNSCOPED_INFO("direct-generic lengthInTicks=" << lengthInTicks);
    UNSCOPED_INFO("direct-generic tempoChanges=" << tempoChanges.size());
    UNSCOPED_INFO("direct-generic timeSignatures=" << timeSignatures.size());
    for (size_t i = 0; i < timeSignatures.size(); ++i)
    {
        UNSCOPED_INFO(
            "direct-generic timesig[" << i << "] tick=" << timeSignatures[i].tick
            << " num=" << timeSignatures[i].numerator
            << " den=" << timeSignatures[i].denominator
        );
    }

    const auto initialTempo = tempoChanges.empty() ? 120.0 : tempoChanges[0].second;
    sequence->setInitialTempo(initialTempo);
    if (!transport->isTempoSourceSequence())
    {
        transport->setTempo(initialTempo);
    }

    for (size_t i = 1; i < tempoChanges.size(); ++i)
    {
        const auto ratio = tempoChanges[i].second / initialTempo;
        sequence->addTempoChangeEvent(
            tempoChanges[i].first,
            static_cast<int>(ratio * 1000.0)
        );
    }

    if (timeSignatures.empty())
    {
        timeSignatures.push_back(
            TimeSigChange{0, mpc::DefaultTimeSigNumerator, mpc::DefaultTimeSigDenominator}
        );
    }

    lengthInTicks = std::max(lengthInTicks, 1);
    int accumLength = 0;
    int barCounter = 0;
    for (size_t i = 0; i < timeSignatures.size(); ++i)
    {
        const auto current = timeSignatures[i];
        const auto nextTick =
            i + 1 < timeSignatures.size() ? timeSignatures[i + 1].tick : lengthInTicks;

        while (accumLength < nextTick)
        {
            sequence->setTimeSignature(barCounter, current.numerator, current.denominator);
            const auto newDenTicks = 96 * (4.0 / current.denominator);
            const auto barLength = static_cast<int>(newDenTicks * current.numerator);
            accumLength += barLength;
            barCounter++;
        }
    }

    sequence->setLastBarIndex(barCounter - 1);
    sequence->setFirstLoopBarIndex(mpc::BarIndex(-1));
    sequence->setLastLoopBarIndex(mpc::EndOfSequence);

    const std::string trackDataPrefix = "TRACK DATA:";

    for (size_t i = 0; i < parsed.tracks()->size() && i < 64; ++i)
    {
        auto resolvedTrackIndex = static_cast<int>(i);
        auto deviceIndex = 0;
        auto busType = mpc::sequencer::BusType::DRUM1;

        for (const auto& eventPtr : *parsed.tracks()->at(i)->events()->event())
        {
            const auto* meta = eventPtr->meta_event_body();
            if (meta == nullptr || meta->meta_type() != meta_type_t::META_TYPE_ENUM_TEXT_EVENT)
            {
                continue;
            }

            const auto body = asciiBody(meta->body());
            if (body.rfind(trackDataPrefix, 0) != 0)
            {
                continue;
            }

            const auto payload = body.substr(trackDataPrefix.size());
            resolvedTrackIndex = std::stoi(payload.substr(0, 2));

            const auto deviceIndexStr = payload.substr(2, 2);
            if (deviceIndexStr != "C0")
            {
                deviceIndex = std::stoi(deviceIndexStr, nullptr, 16) - std::stoi(std::string("E0"), nullptr, 16) + 1;
            }

            if (payload.size() >= 16)
            {
                busType = mpc::sequencer::busIndexToBusType(std::stoi(payload.substr(14, 2), nullptr, 16));
            }
            break;
        }

        if (resolvedTrackIndex < 0 || resolvedTrackIndex > mpc::Mpc2000XlSpecs::LAST_TRACK_INDEX)
        {
            continue;
        }

        auto track = sequence->getTrack(resolvedTrackIndex);
        track->setBusType(busType);
        track->setDeviceIndex(deviceIndex);
        std::string pendingTrackName;
        bool hasPendingTrackName = false;
        bool hasImportedEvents = false;

        struct OpenNote
        {
            mpc::sequencer::EventData event;
            bool isClosed = false;
        };

        std::vector<OpenNote> openNotes;
        std::vector<std::pair<int, int>> noteOffs;

        int absoluteTick = 0;
        for (const auto& eventPtr : *parsed.tracks()->at(i)->events()->event())
        {
            absoluteTick += eventPtr->v_time()->value();

            if (const auto* meta = eventPtr->meta_event_body(); meta != nullptr)
            {
                if (meta->meta_type() == meta_type_t::META_TYPE_ENUM_SEQUENCE_TRACK_NAME)
                {
                    const auto body = asciiBody(meta->body());
                    if (!body.empty())
                    {
                        pendingTrackName = body.substr(0, mpc::Mpc2000XlSpecs::MAX_TRACK_NAME_LENGTH);
                        hasPendingTrackName = true;
                    }
                }
                continue;
            }

            switch (eventPtr->event_type())
            {
                case 0x80:
                {
                    const auto* noteOff =
                        dynamic_cast<midi_t::note_off_event_t*>(eventPtr->event_body());
                    if (noteOff != nullptr)
                    {
                        noteOffs.push_back({noteOff->note(), absoluteTick});
                    }
                    break;
                }
                case 0x90:
                {
                    const auto* noteOn =
                        dynamic_cast<midi_t::note_on_event_t*>(eventPtr->event_body());
                    if (noteOn == nullptr)
                    {
                        break;
                    }
                    mpc::sequencer::EventData ne;
                    ne.type = mpc::sequencer::EventType::NoteOn;
                    ne.noteNumber = mpc::NoteNumber(noteOn->note());
                    ne.tick = absoluteTick;
                    ne.velocity = mpc::Velocity(noteOn->velocity());
                    ne.noteVariationType = mpc::NoteVariationTypeTune;
                    ne.noteVariationValue = mpc::DefaultNoteVariationValue;
                    ne.duration = mpc::Duration(24);
                    openNotes.push_back(OpenNote{ne});
                    break;
                }
                case 0xA0:
                {
                    const auto* noteAftertouch =
                        dynamic_cast<midi_t::polyphonic_pressure_event_t*>(eventPtr->event_body());
                    if (noteAftertouch == nullptr)
                    {
                        break;
                    }
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::PolyPressure;
                    e.tick = absoluteTick;
                    e.noteNumber = mpc::NoteNumber(noteAftertouch->note());
                    e.amount = noteAftertouch->pressure();
                    track->acquireAndInsertEvent(e);
                    hasImportedEvents = true;
                    break;
                }
                case 0xB0:
                {
                    const auto* controller =
                        dynamic_cast<midi_t::controller_event_t*>(eventPtr->event_body());
                    if (controller == nullptr)
                    {
                        break;
                    }
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::ControlChange;
                    e.tick = absoluteTick;
                    e.controllerNumber = controller->controller();
                    e.controllerValue = controller->value();
                    track->acquireAndInsertEvent(e);
                    hasImportedEvents = true;
                    break;
                }
                case 0xC0:
                {
                    const auto* programChange =
                        dynamic_cast<midi_t::program_change_event_t*>(eventPtr->event_body());
                    if (programChange == nullptr)
                    {
                        break;
                    }
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::ProgramChange;
                    e.tick = absoluteTick;
                    e.programChangeProgramIndex = mpc::ProgramIndex(programChange->program());
                    track->acquireAndInsertEvent(e);
                    hasImportedEvents = true;
                    break;
                }
                case 0xD0:
                {
                    const auto* channelPressure =
                        dynamic_cast<midi_t::channel_pressure_event_t*>(eventPtr->event_body());
                    if (channelPressure == nullptr)
                    {
                        break;
                    }
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::ChannelPressure;
                    e.tick = absoluteTick;
                    e.amount = channelPressure->pressure();
                    track->acquireAndInsertEvent(e);
                    hasImportedEvents = true;
                    break;
                }
                case 0xE0:
                {
                    auto* pitchBend =
                        dynamic_cast<midi_t::pitch_bend_event_t*>(eventPtr->event_body());
                    if (pitchBend == nullptr)
                    {
                        break;
                    }
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::PitchBend;
                    e.tick = absoluteTick;
                    e.amount = pitchBend->bend_value();
                    track->acquireAndInsertEvent(e);
                    hasImportedEvents = true;
                    break;
                }
                case 0xF0:
                {
                    const auto* sysEx = eventPtr->sysex_body();
                    if (sysEx == nullptr)
                    {
                        break;
                    }
                    const auto& data = sysEx->data();
                    if (data.size() == 8 &&
                        static_cast<unsigned char>(data[0]) == 71 &&
                        static_cast<unsigned char>(data[1]) == 0 &&
                        static_cast<unsigned char>(data[2]) == 68 &&
                        static_cast<unsigned char>(data[3]) == 69 &&
                        static_cast<unsigned char>(data[7]) == 247)
                    {
                        mpc::sequencer::EventData e;
                        e.type = mpc::sequencer::EventType::Mixer;
                        e.tick = absoluteTick;
                        e.mixerParameter = static_cast<unsigned char>(data[4]) - 1;
                        e.mixerPad = static_cast<unsigned char>(data[5]);
                        e.mixerValue = static_cast<unsigned char>(data[6]);
                        track->acquireAndInsertEvent(e);
                    }
                    else
                    {
                        mpc::sequencer::EventData e;
                        e.type = mpc::sequencer::EventType::SystemExclusive;
                        e.tick = absoluteTick;
                        track->acquireAndInsertEvent(e);
                    }
                    hasImportedEvents = true;
                    break;
                }
                default:
                    break;
            }
        }

        for (auto& on : openNotes)
        {
            for (auto it = noteOffs.begin(); it != noteOffs.end(); ++it)
            {
                if (it->first == on.event.noteNumber && it->second >= on.event.tick)
                {
                    on.event.duration = mpc::Duration(it->second - on.event.tick);
                    noteOffs.erase(it);
                    break;
                }
            }

            track->acquireAndInsertEvent(on.event);
            hasImportedEvents = true;
        }

        if (hasImportedEvents)
        {
            track->setUsedIfCurrentlyUnused();
        }

        if (hasPendingTrackName)
        {
            track->setName(pendingTrackName);
        }
    }

    auto stateManager = mpc.getSequencer()->getStateManager();
    stateManager->drainQueue();
    return sequence;
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

void requireDetailedNote(
    const std::shared_ptr<mpc::sequencer::Track>& track,
    size_t noteIndex,
    int tick,
    int note,
    int velocity,
    int duration,
    int variationType = mpc::NoteVariationTypeTune,
    int variationValue = mpc::DefaultNoteVariationValue)
{
    const auto notes = collectDetailedParsedNotes(track);
    REQUIRE(noteIndex < notes.size());
    const auto& actual = notes[noteIndex];
    REQUIRE(actual.tick == tick);
    REQUIRE(actual.note == note);
    REQUIRE(actual.velocity == velocity);
    REQUIRE(actual.duration == duration);
    REQUIRE(actual.variationType == variationType);
    REQUIRE(actual.variationValue == variationValue);
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

TEST_CASE("Kaitai standard MIDI loads FRUTZLE with expected semantics", "[kaitai-mid]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/FRUTZLE.MID");
    std::vector<char> bytes(file.begin(), file.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "FRUTZLE.MID");

    REQUIRE(sequence->getName() == "Sequence01      ");
    REQUIRE(sequence->getBarCount() == 4);
    REQUIRE(sequence->getInitialTempo() == 96.0);

    const auto track0 = sequence->getTrack(0);
    REQUIRE(track0->isUsed());
    REQUIRE(track0->getName() == "Track-01        ");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::DRUM1);
    REQUIRE(track0->getDeviceIndex() == 0);
    REQUIRE(track0->getEvents().size() == 18);
    requireDetailedNote(track0, 0, 0, 37, 127, 13);
    requireDetailedNote(track0, 3, 288, 37, 16, 12);
    requireDetailedNote(track0, 17, 1440, 36, 127, 0);

    const auto track4 = sequence->getTrack(4);
    REQUIRE(track4->isUsed());
    REQUIRE(track4->getName() == "Track-05        ");
    REQUIRE(track4->getEvents().size() == 32);
    requireDetailedNote(track4, 0, 0, 48, 96, 3);
    requireDetailedNote(track4, 15, 722, 47, 91, 8);
    requireDetailedNote(track4, 31, 1490, 47, 44, 5);

    const auto track8 = sequence->getTrack(8);
    REQUIRE(track8->isUsed());
    REQUIRE(track8->getName() == "Track-09        ");
    REQUIRE(track8->getEvents().size() == 6);
    requireDetailedNote(track8, 0, 3, 82, 127, 180);
    requireDetailedNote(track8, 5, 725, 82, 127, 31);
}

TEST_CASE("Kaitai standard MIDI loads HWIF316 with expected semantics", "[kaitai-mid]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
    std::vector<char> bytes(file.begin(), file.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "HWIF316.MID");

    REQUIRE(sequence->getName() == "Sequence100");
    REQUIRE(sequence->getBarCount() == 16);
    REQUIRE(std::abs(sequence->getInitialTempo() - 125.0) < 0.001);

    const auto track0 = sequence->getTrack(0);
    REQUIRE_FALSE(track0->isUsed());
    REQUIRE(track0->getName() == "HWIF v3 t1 p6");
    REQUIRE(track0->getEvents().empty());

    const auto track1 = sequence->getTrack(1);
    REQUIRE(track1->isUsed());
    REQUIRE(track1->getName() == "Jupiter ARP? out");
    REQUIRE(track1->getEvents().size() == 111);
    requireDetailedNote(track1, 0, 0, 67, 127, 25);
    requireDetailedNote(track1, 31, 1537, 67, 110, 25);
    requireDetailedNote(track1, 110, 5377, 67, 92, 24);

    const auto track2 = sequence->getTrack(2);
    REQUIRE(track2->isUsed());
    REQUIRE(track2->getName() == "JUNO UNISON CHOR");
    REQUIRE(track2->getEvents().size() == 118);
    requireDetailedNote(track2, 0, 1, 55, 73, 24);
    requireDetailedNote(track2, 57, 3073, 55, 73, 24);
    requireDetailedNote(track2, 117, 6144, 65, 0, 24);

    const auto track3 = sequence->getTrack(3);
    REQUIRE(track3->isUsed());
    REQUIRE(track3->getName() == " JD880 pad");
    REQUIRE(track3->getEvents().size() == 48);
    requireDetailedNote(track3, 0, 1, 70, 78, 24);
    requireDetailedNote(track3, 24, 3074, 67, 81, 24);
    requireDetailedNote(track3, 47, 5486, 67, 0, 24);
}

TEST_CASE("Kaitai standard MIDI loads HWIF6-0V2 with expected semantics", "[kaitai-mid]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF6-0V2.MID");
    std::vector<char> bytes(file.begin(), file.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    auto sequence = loadSequenceWithMidIo(mpc, bytes, "HWIF6-0V2.MID");

    REQUIRE(sequence->getName() == "Sequence100");
    REQUIRE(sequence->getBarCount() == 16);
    REQUIRE(std::abs(sequence->getInitialTempo() - 125.0) < 0.001);

    const auto track0 = sequence->getTrack(0);
    REQUIRE(track0->isUsed());
    REQUIRE(track0->getName() == "HWIF6-0v2");
    REQUIRE(track0->getEvents().size() == 204);
    requireDetailedNote(track0, 0, 0, 67, 114, 24);
    requireDetailedNote(track0, 52, 1536, 67, 83, 24);
    requireDetailedNote(track0, 104, 3072, 67, 18, 24);
    requireDetailedNote(track0, 200, 6144, 79, 93, 24);
}

TEST_CASE("Kaitai standard MIDI parses and rewrites HWIF6-0V2", "[.][kaitai-mid-direct]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF6-0V2.MID");
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
}

TEST_CASE("Kaitai standard MIDI parses and rewrites HWIF316", "[.][kaitai-mid-direct]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
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
}

TEST_CASE("Kaitai raw MIDI timeline exposes expected HWIF316 track 0 events", "[.][kaitai-mid-direct]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
    std::vector<char> bytes(file.begin(), file.end());

    const auto kaitai = collectKaitaiTimeline(bytes, 0);

    REQUIRE(kaitai.size() == 3);
    REQUIRE(kaitai[0] == MidiTimelineEventSnapshot{0, "track_name", 13, 0});
    REQUIRE(kaitai[1] == MidiTimelineEventSnapshot{0, "tempo", 1250, 0});
    REQUIRE(kaitai[2] == MidiTimelineEventSnapshot{0, "other", 252, 0});
}

TEST_CASE("Kaitai raw MIDI timeline exposes expected HWIF316 track 3 events", "[.][kaitai-mid-direct]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
    std::vector<char> bytes(file.begin(), file.end());

    const auto kaitai = collectKaitaiTimeline(bytes, 3);

    REQUIRE(kaitai.size() == 49);
    REQUIRE(kaitai[0] == MidiTimelineEventSnapshot{0, "track_name", 10, 0});
    REQUIRE(kaitai[1] == MidiTimelineEventSnapshot{1, "note_on", 70, 78});
    REQUIRE(kaitai[2] == MidiTimelineEventSnapshot{1, "note_on", 74, 67});
    REQUIRE(kaitai[3] == MidiTimelineEventSnapshot{2, "note_on", 67, 81});
    REQUIRE(kaitai[24] == MidiTimelineEventSnapshot{3074, "note_on", 67, 81});
    REQUIRE(kaitai[48] == MidiTimelineEventSnapshot{5486, "note_on", 67, 0});
}

TEST_CASE("Kaitai first raw event on HWIF316 track 0 is a meta event", "[.][kaitai-mid-direct]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/MidiFile/HWIF316.MID");
    std::vector<char> bytes(file.begin(), file.end());

    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    midi_t parsed(&parseIo);
    parsed._read();

    const auto& firstEvent = parsed.tracks()->at(0)->events()->event()->at(0);
    UNSCOPED_INFO("first delta=" << firstEvent->v_time()->value());
    UNSCOPED_INFO("first using_running_status=" << firstEvent->using_running_status());
    UNSCOPED_INFO("first status_byte_lookahead=" << static_cast<int>(firstEvent->status_byte_lookahead()));
    if (!firstEvent->_is_null_event_header())
    {
        UNSCOPED_INFO("first event_header=" << static_cast<int>(firstEvent->event_header()));
    }
    UNSCOPED_INFO("first event_type=" << firstEvent->event_type());
    REQUIRE(firstEvent->meta_event_body() != nullptr);
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
