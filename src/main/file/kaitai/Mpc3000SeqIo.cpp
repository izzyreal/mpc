#include "file/kaitai/Mpc3000SeqIo.hpp"

#include "Mpc.hpp"
#include "MpcSpecs.hpp"
#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/generated/mpc60_seq_v2.h"
#include "file/kaitai/generated/mpc3000_seq_v3.h"
#include "sequencer/BusType.hpp"
#include "sequencer/EventData.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"
#include "utils/SimpleAction.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <sstream>
#include <vector>

using namespace mpc::file::kaitai;

namespace
{
using legacy_event_t = mpc3000_seq_v3_t::event_t;
using legacy_track_header_t = mpc3000_seq_v3_t::track_header_t;
using legacy_tempo_change_t = mpc3000_seq_v3_t::tempo_change_t;

constexpr size_t kFixedPreludeSize =
    2 + 37 + 5 + (64 * 4) + 2 + 15 + 3 + 16 + 1 + 1 + 1;
constexpr size_t kTrackHeaderSize = 24;
constexpr size_t kTempoChangeSize = 6;

struct ParsedSequenceView
{
    std::string sequenceName;
    int barCount;
    double headerTempo;
    bool loopEnabled;
    int loopToBarNumber;
    int numberOfTrackHeaders;
    int numberOfTempoChanges;
    std::optional<size_t> eventsStartOffset;
    const std::vector<std::unique_ptr<legacy_track_header_t>> *trackHeaders;
    const std::vector<std::unique_ptr<legacy_tempo_change_t>> *tempoChanges;
    const std::vector<std::unique_ptr<legacy_event_t>> *parsedEvents;
};

struct TempoChangeData
{
    int tick;
    double tempo;
};

uint16_t readU2Le(const std::vector<char> &bytes, const size_t pos)
{
    return static_cast<uint16_t>(
        static_cast<unsigned char>(bytes.at(pos)) |
        (static_cast<unsigned char>(bytes.at(pos + 1)) << 8));
}

std::string trimAscii(const std::string &value)
{
    return mpc::StrUtil::trim(value);
}

std::vector<uint8_t> payloadWithoutLookahead(
    const std::vector<char> &bytes,
    size_t &pos)
{
    std::vector<uint8_t> payload;

    while (pos < bytes.size())
    {
        const auto byte = static_cast<uint8_t>(bytes[pos++]);
        if (byte > 0x7F)
        {
            if (!payload.empty())
            {
                --pos;
                break;
            }

            payload.push_back(byte);
            break;
        }

        payload.push_back(byte);
    }

    return payload;
}

bool isMixerSysex(const std::vector<uint8_t> &payload)
{
    return payload.size() >= 7 && payload[0] == 71 && payload[1] == 0 &&
           payload[2] == 68 && payload[3] == 69;
}

mpc::TrackIndex resolveTrackIndex(const uint8_t rawTrackNumber)
{
    return mpc::TrackIndex(std::clamp(static_cast<int>(rawTrackNumber) - 1, 0,
                                      mpc::Mpc2000XlSpecs::TRACK_COUNT - 1));
}

void applyTrackHeader(
    const legacy_track_header_t &header,
    const std::shared_ptr<mpc::sequencer::Track> &track)
{
    using mpc::sequencer::BusType;

    track->setOn(!header.track_mute(), false);
    track->setBusType(header.drum_track() ? BusType::DRUM1 : BusType::MIDI,
                      false);

    if (!header.drum_track())
    {
        track->setDeviceIndex(
            std::clamp(static_cast<int>(header.primary_port_channel_assignment()),
                       0, 31),
            false);
    }

    track->setProgramChange(header.program_change_number(), false);

    const auto trackName = trimAscii(header.track_name());
    if (header.track_in_use())
    {
        track->setUsedIfCurrentlyUnused(
            mpc::utils::SimpleAction(
                [track, trackName]
                {
                    if (!trackName.empty())
                    {
                        track->setName(trackName);
                    }
                }));
    }
    else if (!trackName.empty())
    {
        track->setName(trackName);
    }
}

bool isKnownEventStatus(const uint8_t status)
{
    switch (status)
    {
        case 0x88:
        case 0x90:
        case 0x98:
        case 0xA0:
        case 0xA8:
        case 0xB0:
        case 0xC0:
        case 0xD0:
        case 0xE0:
        case 0xE8:
        case 0xF0:
        case 0xFF:
            return true;
        default:
            return false;
    }
}

size_t eventStreamStartOffset(
    const ParsedSequenceView &parsed,
    const std::vector<char> &bytes)
{
    if (parsed.eventsStartOffset.has_value())
    {
        return std::min(*parsed.eventsStartOffset, bytes.size());
    }

    const auto start = kFixedPreludeSize +
                       (parsed.numberOfTrackHeaders * kTrackHeaderSize) +
                       (parsed.numberOfTempoChanges * kTempoChangeSize);
    return std::min(start, bytes.size());
}

std::vector<TempoChangeData> decodeTempoChanges(const ParsedSequenceView &parsed)
{
    std::vector<TempoChangeData> result;
    const auto *tempoChanges = parsed.tempoChanges;
    if (tempoChanges == nullptr)
    {
        return result;
    }

    result.reserve(tempoChanges->size());

    for (const auto &tempoChangePtr : *tempoChanges)
    {
        if (!tempoChangePtr)
        {
            continue;
        }

        result.push_back(
            TempoChangeData{
                static_cast<int>(tempoChangePtr->ticks_from_sequence_start()),
                parsed.headerTempo * (tempoChangePtr->factor_percentage() / 100.0)
            });
    }

    return result;
}

ParsedSequenceView makeParsedSequenceView(const mpc3000_seq_v3_t &parsed)
{
    return ParsedSequenceView{
        trimAscii(parsed.sequence_header()->sequence_name()),
        std::max(1, static_cast<int>(parsed.sequence_header()->number_of_bars())),
        static_cast<double>(parsed.sequence_header()->tempo()) / 10.0,
        parsed.sequence_header()->loop_to_bar() == mpc3000_seq_v3_t::OFF_ON_TRUE,
        std::max(0, static_cast<int>(parsed.sequence_header()->loop_to_bar_number())),
        static_cast<int>(parsed.num_track_headers()),
        static_cast<int>(parsed.num_tempo_changes()),
        std::nullopt,
        parsed.track_headers(),
        parsed.tempo_changes(),
        nullptr,
    };
}

ParsedSequenceView makeParsedSequenceView(mpc60_seq_body_t::sequence_t &sequence)
{
    const auto *header = sequence.sequence_header();
    return ParsedSequenceView{
        trimAscii(header->sequence_name()),
        std::max(1, static_cast<int>(header->number_of_bars())),
        static_cast<double>(header->tempo()) / 10.0,
        header->loop_to_bar() == mpc60_seq_body_t::OFF_ON_TRUE,
        std::max(0, static_cast<int>(header->loop_to_bar_number())),
        static_cast<int>(header->num_track_headers()),
        static_cast<int>(header->num_tempo_changes()),
        static_cast<size_t>(sequence.events_start()),
        sequence.track_headers(),
        sequence.tempo_changes(),
        sequence.events(),
    };
}

ParsedSequenceView makeParsedSequenceView(const mpc60_seq_v2_t &parsed)
{
    auto *sequence =
        const_cast<mpc60_seq_body_t::sequence_t *>(parsed.sequence());
    return makeParsedSequenceView(*sequence);
}

void loadEventsFromParsedObjects(
    const ParsedSequenceView &parsed,
    const std::shared_ptr<mpc::sequencer::Sequence> &newSeq)
{
    int absoluteTick = 0;

    for (const auto &eventPtr : *parsed.parsedEvents)
    {
        auto &event = *eventPtr;
        const auto status = event.status();

        switch (status)
        {
            case 0x88:
            {
                auto *deltaTime =
                    dynamic_cast<mpc3000_seq_v3_t::delta_time_event_t *>(
                        event.event_body());
                if (deltaTime != nullptr)
                {
                    absoluteTick += deltaTime->delta_time();
                }
                break;
            }
            case 0xA8:
            {
                auto *barEvent =
                    dynamic_cast<mpc3000_seq_v3_t::bar_event_t *>(
                        event.event_body());
                if (barEvent != nullptr)
                {
                    const auto barIndex = std::max(0, barEvent->bar_number() - 1);
                    newSeq->setTimeSignature(
                        barIndex, barEvent->numerator(), barEvent->denominator());
                }
                break;
            }
            case 0xA0:
            {
                auto *polyPressure =
                    dynamic_cast<mpc3000_seq_v3_t::poly_pressure_event_t *>(
                        event.event_body());
                if (polyPressure != nullptr)
                {
                    const auto trackIndex = resolveTrackIndex(event.track_number());
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::PolyPressure;
                    e.tick = absoluteTick;
                    e.noteNumber = mpc::NoteNumber(polyPressure->note());
                    e.amount = polyPressure->pressure();
                    newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xB0:
            {
                auto *controlChange =
                    dynamic_cast<mpc3000_seq_v3_t::control_change_event_t *>(
                        event.event_body());
                if (controlChange != nullptr)
                {
                    const auto trackIndex = resolveTrackIndex(event.track_number());
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::ControlChange;
                    e.tick = absoluteTick;
                    e.controllerNumber =
                        static_cast<uint8_t>(controlChange->controller());
                    e.controllerValue = controlChange->value();
                    newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xC0:
            {
                auto *programChange =
                    dynamic_cast<mpc3000_seq_v3_t::program_change_event_t *>(
                        event.event_body());
                if (programChange != nullptr)
                {
                    const auto trackIndex = resolveTrackIndex(event.track_number());
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::ProgramChange;
                    e.tick = absoluteTick;
                    e.programChangeProgramIndex =
                        mpc::ProgramIndex(programChange->program());
                    newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xD0:
            {
                auto *chPressure =
                    dynamic_cast<mpc3000_seq_v3_t::ch_pressure_event_t *>(
                        event.event_body());
                if (chPressure != nullptr)
                {
                    const auto trackIndex = resolveTrackIndex(event.track_number());
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::ChannelPressure;
                    e.tick = absoluteTick;
                    e.amount = chPressure->pressure();
                    newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xE0:
            {
                auto *pitchBend =
                    dynamic_cast<mpc3000_seq_v3_t::pitch_bend_event_t *>(
                        event.event_body());
                if (pitchBend != nullptr)
                {
                    const auto trackIndex = resolveTrackIndex(event.track_number());
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::PitchBend;
                    e.tick = absoluteTick;
                    e.amount = pitchBend->corrected_pitch_bend_amount();
                    newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xF0:
            {
                const auto trackIndex = resolveTrackIndex(event.track_number());
                auto track = newSeq->getTrack(trackIndex);

                if (event.mixer_event() != nullptr)
                {
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::Mixer;
                    e.tick = absoluteTick;
                    e.mixerParameter =
                        static_cast<int8_t>(event.mixer_event()->param() - 1);
                    e.mixerPad = event.mixer_event()->pad_index();
                    e.mixerValue = event.mixer_event()->value();
                    track->acquireAndInsertEvent(e);
                }
                else if (event.system_exclusive_body() != nullptr &&
                         !event.system_exclusive_body()->empty())
                {
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::SystemExclusive;
                    e.tick = absoluteTick;
                    e.sysExByteA = event.system_exclusive_body()->at(0);
                    e.sysExByteB =
                        event.system_exclusive_body()->size() > 1
                            ? event.system_exclusive_body()->at(1)
                            : 0;
                    track->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xE8:
                break;
            case 0x90:
            case 0x98:
            {
                auto *noteEvent =
                    dynamic_cast<mpc3000_seq_v3_t::note_event_t *>(
                        event.event_body());
                if (noteEvent != nullptr)
                {
                    const auto trackIndex = resolveTrackIndex(event.track_number());
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::NoteOn;
                    e.tick = absoluteTick;
                    e.noteNumber = mpc::NoteNumber(noteEvent->note_number());
                    e.velocity = mpc::Velocity(noteEvent->velocity());
                    e.noteVariationValue =
                        mpc::NoteVariationValue(noteEvent->note_variation_value());
                    e.duration = mpc::Duration(noteEvent->duration());
                    e.noteVariationType =
                        status == 0x90
                            ? mpc::NoteVariationTypeTune
                            : mpc::NoteVariationType(status - 0x98);
                    newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                }
                break;
            }
            default:
                break;
        }
    }
}

void loadEventsFromRawBytes(
    const ParsedSequenceView &parsed,
    const std::vector<char> &bytes,
    const std::shared_ptr<mpc::sequencer::Sequence> &newSeq)
{
    int absoluteTick = 0;
    size_t pos = eventStreamStartOffset(parsed, bytes);

    if (pos >= bytes.size())
    {
        return;
    }

    uint8_t status = static_cast<uint8_t>(bytes[pos++]);
    if (status == 0xFF && pos < bytes.size() &&
        isKnownEventStatus(static_cast<uint8_t>(bytes[pos])))
    {
        status = static_cast<uint8_t>(bytes[pos++]);
    }

    while (true)
    {
        if (!isKnownEventStatus(status))
        {
            break;
        }

        if (status == 0xFF)
        {
            break;
        }

        auto readTrack = [&]() -> mpc::TrackIndex
        {
            return resolveTrackIndex(static_cast<uint8_t>(bytes.at(pos++)));
        };

        switch (status)
        {
            case 0x88:
                absoluteTick += readU2Le(bytes, pos);
                pos += 2;
                break;
            case 0xA8:
            {
                const auto barNumber =
                    static_cast<int>(bytes.at(pos)) |
                    (static_cast<int>(bytes.at(pos + 1)) << 7);
                const auto numerator =
                    static_cast<unsigned char>(bytes.at(pos + 2));
                const auto denominator =
                    static_cast<unsigned char>(bytes.at(pos + 3));
                pos += 4;
                const auto barIndex = std::max(0, barNumber - 1);
                newSeq->setTimeSignature(barIndex, numerator, denominator);
                break;
            }
            case 0xA0:
            {
                const auto trackIndex = readTrack();
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::PolyPressure;
                e.tick = absoluteTick;
                e.noteNumber =
                    mpc::NoteNumber(static_cast<unsigned char>(bytes.at(pos++)));
                e.amount = static_cast<unsigned char>(bytes.at(pos++));
                newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                break;
            }
            case 0xB0:
            {
                const auto trackIndex = readTrack();
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::ControlChange;
                e.tick = absoluteTick;
                e.controllerNumber = static_cast<unsigned char>(bytes.at(pos++));
                e.controllerValue = static_cast<unsigned char>(bytes.at(pos++));
                newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                break;
            }
            case 0xC0:
            {
                const auto trackIndex = readTrack();
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::ProgramChange;
                e.tick = absoluteTick;
                e.programChangeProgramIndex =
                    mpc::ProgramIndex(static_cast<unsigned char>(bytes.at(pos++)));
                newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                break;
            }
            case 0xD0:
            {
                const auto trackIndex = readTrack();
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::ChannelPressure;
                e.tick = absoluteTick;
                e.amount = static_cast<unsigned char>(bytes.at(pos++));
                newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                break;
            }
            case 0xE0:
            {
                const auto trackIndex = readTrack();
                const auto bits1 = static_cast<unsigned char>(bytes.at(pos++));
                const auto bits2 = static_cast<unsigned char>(bytes.at(pos++));
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::PitchBend;
                e.tick = absoluteTick;
                e.amount = static_cast<int16_t>((bits1 + (bits2 << 7)) - 8192);
                newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                break;
            }
            case 0xF0:
            {
                const auto trackIndex = readTrack();
                const auto payload = payloadWithoutLookahead(bytes, pos);
                auto track = newSeq->getTrack(trackIndex);

                if (isMixerSysex(payload))
                {
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::Mixer;
                    e.tick = absoluteTick;
                    e.mixerParameter = static_cast<int8_t>(payload.at(4) - 1);
                    e.mixerPad = payload.at(5);
                    e.mixerValue = payload.at(6);
                    track->acquireAndInsertEvent(e);
                }
                else if (!payload.empty())
                {
                    mpc::sequencer::EventData e;
                    e.type = mpc::sequencer::EventType::SystemExclusive;
                    e.tick = absoluteTick;
                    e.sysExByteA = payload[0];
                    e.sysExByteB = payload.size() > 1 ? payload[1] : 0;
                    track->acquireAndInsertEvent(e);
                }
                break;
            }
            case 0xE8:
                (void)readTrack();
                break;
            case 0x90:
            case 0x98:
            {
                const auto trackIndex = readTrack();
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::NoteOn;
                e.tick = absoluteTick;
                e.noteNumber =
                    mpc::NoteNumber(static_cast<unsigned char>(bytes.at(pos++)));
                e.velocity =
                    mpc::Velocity(static_cast<unsigned char>(bytes.at(pos++)));
                e.noteVariationValue = mpc::NoteVariationValue(
                    static_cast<unsigned char>(bytes.at(pos++)));
                e.duration = mpc::Duration(
                    static_cast<unsigned char>(bytes.at(pos)) |
                    (static_cast<unsigned char>(bytes.at(pos + 1)) << 7));
                pos += 2;
                e.noteVariationType =
                    status == 0x90
                        ? mpc::NoteVariationTypeTune
                        : mpc::NoteVariationType(status - 0x98);
                newSeq->getTrack(trackIndex)->acquireAndInsertEvent(e);
                break;
            }
            default:
                break;
        }

        if (pos >= bytes.size())
        {
            break;
        }

        status = static_cast<uint8_t>(bytes[pos++]);
        if (status == 0xFF && pos < bytes.size() &&
            isKnownEventStatus(static_cast<uint8_t>(bytes[pos])))
        {
            status = static_cast<uint8_t>(bytes[pos++]);
        }

        if (status == 0xFF && pos >= bytes.size())
        {
            break;
        }
        if (status == 0x00 && pos >= bytes.size())
        {
            break;
        }
    }
}

sequence_or_error loadParsedSequence(
    mpc::Mpc &mpc,
    const std::vector<char> &bytes,
    const std::string &fileNameWithoutExtension,
    const ParsedSequenceView &parsed)
{
    auto newSeq = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);

    newSeq->init(parsed.barCount - 1);
    mpc.getSequencer()->getStateManager()->drainQueue();

    const auto tempoChanges = decodeTempoChanges(parsed);

    auto initialTempo = parsed.headerTempo;
    bool consumedInitialTempoChange = false;
    for (const auto &tempoChange : tempoChanges)
    {
        if (tempoChange.tick == 0)
        {
            initialTempo = tempoChange.tempo;
            consumedInitialTempoChange = true;
            break;
        }
    }

    newSeq->setInitialTempo(initialTempo);
    const auto transport = mpc.getSequencer()->getTransport();
    if (!transport->isTempoSourceSequence())
    {
        transport->setTempo(initialTempo);
    }

    for (const auto &tempoChange : tempoChanges)
    {
        if (consumedInitialTempoChange && tempoChange.tick == 0)
        {
            consumedInitialTempoChange = false;
            continue;
        }

        if (tempoChange.tick <= 0)
        {
            continue;
        }

        const auto ratio = static_cast<int>(
            std::round((tempoChange.tempo / initialTempo) * 1000.0));
        newSeq->addTempoChangeEvent(tempoChange.tick, ratio);
    }

    newSeq->setName(
        parsed.sequenceName.empty() ? fileNameWithoutExtension : parsed.sequenceName);

    newSeq->setFirstLoopBarIndex(mpc::BarIndex(0));
    newSeq->setLoopEnabled(parsed.loopEnabled);
    if (parsed.loopEnabled)
    {
        newSeq->setLastLoopBarIndex(
            mpc::BarIndex(std::max(0, parsed.loopToBarNumber - 1)));
    }
    else
    {
        newSeq->setLastLoopBarIndex(mpc::EndOfSequence);
    }

    std::vector<const legacy_track_header_t *> trackHeaders(
        mpc::Mpc2000XlSpecs::TRACK_COUNT, nullptr);
    for (const auto &headerPtr : *parsed.trackHeaders)
    {
        const auto *header = headerPtr.get();
        if (header->absolute_recorded_track_number() == -1)
        {
            continue;
        }

        const auto trackIndex = std::clamp(
            static_cast<int>(header->user_track_number()) - 1, 0,
            mpc::Mpc2000XlSpecs::TRACK_COUNT - 1);
        trackHeaders[trackIndex] = header;
    }

    if (parsed.parsedEvents != nullptr)
    {
        loadEventsFromParsedObjects(parsed, newSeq);
    }
    else
    {
        loadEventsFromRawBytes(parsed, bytes, newSeq);
    }

    mpc.getSequencer()->getStateManager()->drainQueue();

    for (size_t trackIndex = 0; trackIndex < trackHeaders.size(); ++trackIndex)
    {
        if (trackHeaders[trackIndex] == nullptr)
        {
            continue;
        }

        applyTrackHeader(*trackHeaders[trackIndex], newSeq->getTrack(trackIndex));
    }

    mpc.getSequencer()->getStateManager()->drainQueue();

    return newSeq;
}
} // namespace

sequence_or_error Mpc3000SeqIo::loadMpc60Sequence(
    mpc::Mpc &mpc,
    const mpc60_seq_body_t::sequence_t &sequence,
    const std::string &fileNameWithoutExtension)
{
    auto *mutableSequence =
        const_cast<mpc60_seq_body_t::sequence_t *>(&sequence);
    return loadParsedSequence(
        mpc, {}, fileNameWithoutExtension, makeParsedSequenceView(*mutableSequence));
}

sequence_or_error Mpc3000SeqIo::loadBytes(
    mpc::Mpc &mpc,
    const std::vector<char> &bytes,
    const std::string &fileNameWithoutExtension)
{
    if (bytes.size() < 2)
    {
        return tl::make_unexpected("SEQ file is too short");
    }

    std::stringstream kaitaiStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary);
    ::kaitai::kstream kaitaiIo(&kaitaiStream);

    const auto fileId = static_cast<unsigned char>(bytes[0]);
    const auto version = static_cast<unsigned char>(bytes[1]);

    if (fileId != 0x03)
    {
        return tl::make_unexpected("SEQ first byte is incorrect");
    }

    if (version == 0x02)
    {
        mpc60_seq_v2_t parsed(&kaitaiIo);
        parsed._read();
        return loadParsedSequence(
            mpc, bytes, fileNameWithoutExtension, makeParsedSequenceView(parsed));
    }

    if (version == 0x03)
    {
        mpc3000_seq_v3_t parsed(&kaitaiIo);
        parsed._read();
        return loadParsedSequence(
            mpc, bytes, fileNameWithoutExtension, makeParsedSequenceView(parsed));
    }

    return tl::make_unexpected("Unsupported SEQ file format");
}

sequence_or_error
Mpc3000SeqIo::load(mpc::Mpc &mpc,
                   const std::shared_ptr<mpc::disk::MpcFile> &file)
{
    return loadBytes(mpc, file->getBytes(), file->getNameWithoutExtension());
}
