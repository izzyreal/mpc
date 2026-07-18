#include "AllLoader.hpp"

#include "Mpc.hpp"

#include "StrUtil.hpp"
#include "file/ByteUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "file/kaitai/AllIo.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "file/kaitai/generated/mpc60_all_v2.h"
#include "file/kaitai/generated/mpc3000_all_v3.h"
#include "file/kaitai/generated/mpc3000_seq_v3.h"

#include "disk/MpcFile.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/TrackState.hpp"

#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MidiOutputScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimeDisplayScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "lcdgui/screens/window/LocateScreen.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/OthersScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"

#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <kaitai/kaitaistream.h>
#include <algorithm>
#include <cmath>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace {
using legacy_event_t = mpc3000_seq_v3_t::event_t;
using legacy_track_header_t = mpc3000_seq_v3_t::track_header_t;
using legacy_tempo_change_t = mpc3000_seq_v3_t::tempo_change_t;

struct LegacyParsedSequenceView
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

struct LegacyTempoChangeData
{
    int tick;
    double tempo;
};

enum class all_variant_t
{
    mpc2000xl,
    mpc60_v2,
    mpc3000_v3,
    unknown
};

constexpr size_t kLegacyAllHeaderSize = 6;
constexpr size_t kLegacyAllEmbeddedPreludeSize =
    37 + 5 + (64 * 4) + 2 + 15 + 3 + 16 + 1 + 1 + 1;
constexpr size_t kLegacyStandalonePreludeSize =
    2 + kLegacyAllEmbeddedPreludeSize;
constexpr size_t kMpc3000TrackHeaderSize = 24;
constexpr size_t kMpc3000TempoChangeSize = 6;

bool startsWith(const std::vector<char>& bytes, const std::string_view prefix)
{
    return bytes.size() >= prefix.size() &&
        std::equal(prefix.begin(), prefix.end(), bytes.begin());
}

all_variant_t detectAllVariant(const std::vector<char>& bytes)
{
    if (startsWith(bytes, "MPC2KXL ALL 1.00"))
    {
        return all_variant_t::mpc2000xl;
    }

    if (bytes.size() >= 2 &&
        static_cast<unsigned char>(bytes[0]) == 0x04 &&
        static_cast<unsigned char>(bytes[1]) == 0x02)
    {
        return all_variant_t::mpc60_v2;
    }

    if (bytes.size() >= 2 &&
        static_cast<unsigned char>(bytes[0]) == 0x04 &&
        static_cast<unsigned char>(bytes[1]) == 0x03)
    {
        return all_variant_t::mpc3000_v3;
    }

    return all_variant_t::unknown;
}

std::string bytesUntilNull(const std::string& value)
{
    const auto pos = value.find('\0');
    return pos == std::string::npos ? value : value.substr(0, pos);
}

uint16_t readU2Le(const std::vector<char> &bytes, const size_t pos)
{
    return static_cast<uint16_t>(
        static_cast<unsigned char>(bytes.at(pos)) |
        (static_cast<unsigned char>(bytes.at(pos + 1)) << 8));
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

bool isLegacyMixerSysex(const std::vector<uint8_t> &payload)
{
    return payload.size() >= 7 && payload[0] == 71 && payload[1] == 0 &&
           payload[2] == 68 && payload[3] == 69;
}

mpc::TrackIndex resolveLegacyTrackIndex(const uint8_t rawTrackNumber)
{
    return mpc::TrackIndex(std::clamp(static_cast<int>(rawTrackNumber) - 1, 0,
                                      mpc::Mpc2000XlSpecs::TRACK_COUNT - 1));
}

bool isKnownLegacyEventStatus(const uint8_t status)
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

size_t legacyEventStreamStartOffset(
    const LegacyParsedSequenceView &parsed,
    const std::vector<char> &bytes)
{
    if (parsed.eventsStartOffset.has_value())
    {
        return std::min(*parsed.eventsStartOffset, bytes.size());
    }

    const auto start = kLegacyStandalonePreludeSize +
                       (parsed.numberOfTrackHeaders * kMpc3000TrackHeaderSize) +
                       (parsed.numberOfTempoChanges * kMpc3000TempoChangeSize);
    return std::min(start, bytes.size());
}

std::vector<LegacyTempoChangeData>
decodeLegacyTempoChanges(const LegacyParsedSequenceView &parsed)
{
    std::vector<LegacyTempoChangeData> result;
    if (parsed.tempoChanges == nullptr)
    {
        return result;
    }

    result.reserve(parsed.tempoChanges->size());
    for (const auto &tempoChangePtr : *parsed.tempoChanges)
    {
        if (!tempoChangePtr)
        {
            continue;
        }

        result.push_back(LegacyTempoChangeData{
            static_cast<int>(tempoChangePtr->ticks_from_sequence_start()),
            parsed.headerTempo *
                (tempoChangePtr->factor_percentage() / 100.0)});
    }

    return result;
}

LegacyParsedSequenceView makeLegacyParsedSequenceView(
    const mpc3000_seq_v3_t &parsed)
{
    return LegacyParsedSequenceView{
        mpc::StrUtil::trim(parsed.sequence_header()->sequence_name()),
        std::max(1, static_cast<int>(
                        parsed.sequence_header()->number_of_bars())),
        static_cast<double>(parsed.sequence_header()->tempo()) / 10.0,
        parsed.sequence_header()->loop_to_bar() ==
            mpc3000_seq_v3_t::OFF_ON_TRUE,
        std::max(0, static_cast<int>(
                        parsed.sequence_header()->loop_to_bar_number())),
        static_cast<int>(parsed.num_track_headers()),
        static_cast<int>(parsed.num_tempo_changes()),
        std::nullopt,
        parsed.track_headers(),
        parsed.tempo_changes(),
        nullptr,
    };
}

LegacyParsedSequenceView makeLegacyParsedSequenceView(
    mpc60_seq_body_t::sequence_t &sequence)
{
    const auto *header = sequence.sequence_header();
    return LegacyParsedSequenceView{
        mpc::StrUtil::trim(header->sequence_name()),
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

void addLegacyImportedEvent(
    mpc::sequencer::SequenceTrackEventsSnapshot &events,
    mpc::sequencer::SequenceTrackStatesSnapshot &trackStates,
    const mpc::TrackIndex trackIndex,
    const mpc::sequencer::EventData &event,
    const std::string &defaultTrackName)
{
    events[trackIndex].push_back(event);

    auto &trackState = trackStates[trackIndex];
    trackState.used = true;
    if (trackState.name == "(Unused)")
    {
        trackState.name = defaultTrackName;
    }
}

void applyLegacyTrackHeader(
    const legacy_track_header_t &header,
    mpc::sequencer::TrackState &trackState,
    const std::string &defaultTrackName)
{
    trackState.on = !header.track_mute();
    trackState.busType = header.drum_track()
                             ? mpc::sequencer::BusType::DRUM1
                             : mpc::sequencer::BusType::MIDI;

    if (!header.drum_track())
    {
        trackState.deviceIndex =
            std::clamp(static_cast<int>(
                           header.primary_port_channel_assignment()),
                       0, 31);
    }

    trackState.programChange = header.program_change_number();

    const auto trackName =
        mpc::StrUtil::trim(header.track_name());
    if (header.track_in_use())
    {
        trackState.used = true;
        if (trackName.empty() && trackState.name == "(Unused)")
        {
            trackState.name = defaultTrackName;
        }
        else if (!trackName.empty())
        {
            trackState.name = trackName;
        }
    }
    else if (!trackName.empty())
    {
        trackState.name = trackName;
    }
}

void loadLegacyEventsFromParsedObjects(
    const LegacyParsedSequenceView &parsed,
    const std::shared_ptr<mpc::sequencer::Sequence> &sequence,
    mpc::sequencer::SequenceTrackEventsSnapshot &events,
    mpc::sequencer::SequenceTrackStatesSnapshot &trackStates,
    mpc::sequencer::Sequencer &sequencer)
{
    int absoluteTick = 0;

    for (const auto &eventPtr : *parsed.parsedEvents)
    {
        auto &event = *eventPtr;
        const auto status = event.status();

        if (status == 0x88)
        {
            auto *deltaTime =
                dynamic_cast<mpc3000_seq_v3_t::delta_time_event_t *>(
                    event.event_body());
            if (deltaTime != nullptr)
            {
                absoluteTick += deltaTime->delta_time();
            }
            continue;
        }

        if (status == 0xA8)
        {
            if (auto *barEvent =
                    dynamic_cast<mpc3000_seq_v3_t::bar_event_t *>(
                        event.event_body()))
            {
                const auto barIndex = std::max(0, barEvent->bar_number() - 1);
                sequence->setTimeSignature(
                    barIndex, barEvent->numerator(), barEvent->denominator());
            }
            continue;
        }

        if (status == 0xE8)
        {
            continue;
        }

        const auto trackIndex = resolveLegacyTrackIndex(event.track_number());
        mpc::sequencer::EventData e;
        e.tick = absoluteTick;

        switch (status)
        {
            case 0xA0:
                if (auto *body =
                        dynamic_cast<mpc3000_seq_v3_t::poly_pressure_event_t *>(
                            event.event_body()))
                {
                    e.type = mpc::sequencer::EventType::PolyPressure;
                    e.noteNumber = mpc::NoteNumber(body->note());
                    e.amount = body->pressure();
                }
                break;
            case 0xB0:
                if (auto *body =
                        dynamic_cast<mpc3000_seq_v3_t::control_change_event_t *>(
                            event.event_body()))
                {
                    e.type = mpc::sequencer::EventType::ControlChange;
                    e.controllerNumber = static_cast<uint8_t>(body->controller());
                    e.controllerValue = body->value();
                }
                break;
            case 0xC0:
                if (auto *body =
                        dynamic_cast<mpc3000_seq_v3_t::program_change_event_t *>(
                            event.event_body()))
                {
                    e.type = mpc::sequencer::EventType::ProgramChange;
                    e.programChangeProgramIndex =
                        mpc::ProgramIndex(body->program());
                }
                break;
            case 0xD0:
                if (auto *body =
                        dynamic_cast<mpc3000_seq_v3_t::ch_pressure_event_t *>(
                            event.event_body()))
                {
                    e.type = mpc::sequencer::EventType::ChannelPressure;
                    e.amount = body->pressure();
                }
                break;
            case 0xE0:
                if (auto *body =
                        dynamic_cast<mpc3000_seq_v3_t::pitch_bend_event_t *>(
                            event.event_body()))
                {
                    e.type = mpc::sequencer::EventType::PitchBend;
                    e.amount = body->corrected_pitch_bend_amount();
                }
                break;
            case 0xF0:
                if (event.mixer_event() != nullptr)
                {
                    e.type = mpc::sequencer::EventType::Mixer;
                    e.mixerParameter =
                        static_cast<int8_t>(event.mixer_event()->param() - 1);
                    e.mixerPad = event.mixer_event()->pad_index();
                    e.mixerValue = event.mixer_event()->value();
                }
                else if (event.system_exclusive_body() != nullptr &&
                         !event.system_exclusive_body()->empty())
                {
                    e.type = mpc::sequencer::EventType::SystemExclusive;
                    e.sysExByteA = event.system_exclusive_body()->at(0);
                    e.sysExByteB =
                        event.system_exclusive_body()->size() > 1
                            ? event.system_exclusive_body()->at(1)
                            : 0;
                }
                break;
            case 0x90:
            case 0x98:
                if (auto *body =
                        dynamic_cast<mpc3000_seq_v3_t::note_event_t *>(
                            event.event_body()))
                {
                    e.type = mpc::sequencer::EventType::NoteOn;
                    e.noteNumber = mpc::NoteNumber(body->note_number());
                    e.velocity = mpc::Velocity(body->velocity());
                    e.noteVariationValue =
                        mpc::NoteVariationValue(body->note_variation_value());
                    e.duration = mpc::Duration(body->duration());
                    e.noteVariationType =
                        status == 0x90
                            ? mpc::NoteVariationTypeTune
                            : mpc::NoteVariationType(status - 0x98);
                }
                break;
            default:
                break;
        }

        if (e.type != mpc::sequencer::EventType::Unknown)
        {
            addLegacyImportedEvent(
                events, trackStates, trackIndex, e,
                sequencer.getDefaultTrackName(trackIndex));
        }
    }
}

void loadLegacyEventsFromRawBytes(
    const LegacyParsedSequenceView &parsed,
    const std::vector<char> &bytes,
    const std::shared_ptr<mpc::sequencer::Sequence> &sequence,
    mpc::sequencer::SequenceTrackEventsSnapshot &events,
    mpc::sequencer::SequenceTrackStatesSnapshot &trackStates,
    mpc::sequencer::Sequencer &sequencer)
{
    int absoluteTick = 0;
    size_t pos = legacyEventStreamStartOffset(parsed, bytes);

    if (pos >= bytes.size())
    {
        return;
    }

    uint8_t status = static_cast<uint8_t>(bytes[pos++]);
    if (status == 0xFF && pos < bytes.size() &&
        isKnownLegacyEventStatus(static_cast<uint8_t>(bytes[pos])))
    {
        status = static_cast<uint8_t>(bytes[pos++]);
    }

    while (true)
    {
        if (!isKnownLegacyEventStatus(status) || status == 0xFF)
        {
            break;
        }

        auto readTrack = [&]() -> mpc::TrackIndex
        {
            return resolveLegacyTrackIndex(static_cast<uint8_t>(bytes.at(pos++)));
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
                sequence->setTimeSignature(barIndex, numerator, denominator);
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
                addLegacyImportedEvent(
                    events, trackStates, trackIndex, e,
                    sequencer.getDefaultTrackName(trackIndex));
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
                addLegacyImportedEvent(
                    events, trackStates, trackIndex, e,
                    sequencer.getDefaultTrackName(trackIndex));
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
                addLegacyImportedEvent(
                    events, trackStates, trackIndex, e,
                    sequencer.getDefaultTrackName(trackIndex));
                break;
            }
            case 0xD0:
            {
                const auto trackIndex = readTrack();
                mpc::sequencer::EventData e;
                e.type = mpc::sequencer::EventType::ChannelPressure;
                e.tick = absoluteTick;
                e.amount = static_cast<unsigned char>(bytes.at(pos++));
                addLegacyImportedEvent(
                    events, trackStates, trackIndex, e,
                    sequencer.getDefaultTrackName(trackIndex));
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
                addLegacyImportedEvent(
                    events, trackStates, trackIndex, e,
                    sequencer.getDefaultTrackName(trackIndex));
                break;
            }
            case 0xF0:
            {
                const auto trackIndex = readTrack();
                const auto payload = payloadWithoutLookahead(bytes, pos);
                mpc::sequencer::EventData e;
                e.tick = absoluteTick;

                if (isLegacyMixerSysex(payload))
                {
                    e.type = mpc::sequencer::EventType::Mixer;
                    e.mixerParameter = static_cast<int8_t>(payload.at(4) - 1);
                    e.mixerPad = payload.at(5);
                    e.mixerValue = payload.at(6);
                }
                else if (!payload.empty())
                {
                    e.type = mpc::sequencer::EventType::SystemExclusive;
                    e.sysExByteA = payload[0];
                    e.sysExByteB = payload.size() > 1 ? payload[1] : 0;
                }

                if (e.type != mpc::sequencer::EventType::Unknown)
                {
                    addLegacyImportedEvent(
                        events, trackStates, trackIndex, e,
                        sequencer.getDefaultTrackName(trackIndex));
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
                addLegacyImportedEvent(
                    events, trackStates, trackIndex, e,
                    sequencer.getDefaultTrackName(trackIndex));
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
            isKnownLegacyEventStatus(static_cast<uint8_t>(bytes[pos])))
        {
            status = static_cast<uint8_t>(bytes[pos++]);
        }

        if ((status == 0xFF || status == 0x00) && pos >= bytes.size())
        {
            break;
        }
    }
}

void applyLegacyParsedSequenceToInMemorySequence(
    const LegacyParsedSequenceView &parsed,
    const std::vector<char> &bytes,
    const std::shared_ptr<mpc::sequencer::Sequence> &sequence,
    mpc::sequencer::Sequencer &sequencer,
    mpc::sequencer::SequencerStateManager &stateManager)
{
    sequence->init(parsed.barCount - 1);

    const auto tempoChanges = decodeLegacyTempoChanges(parsed);
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

    sequence->setInitialTempo(initialTempo);
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
        sequence->addTempoChangeEvent(tempoChange.tick, ratio);
    }

    sequence->setName(parsed.sequenceName);
    stateManager.enqueue(mpc::sequencer::SetFirstLoopBarIndex{
        sequence->getSequenceIndex(), mpc::BarIndex(0)});
    stateManager.enqueue(mpc::sequencer::SetLoopEnabled{
        sequence->getSequenceIndex(), parsed.loopEnabled});
    stateManager.enqueue(mpc::sequencer::SetLastLoopBarIndex{
        sequence->getSequenceIndex(),
        parsed.loopEnabled
            ? mpc::BarIndex(std::max(0, parsed.loopToBarNumber - 1))
            : mpc::EndOfSequence});

    const auto sequenceIndex = sequence->getSequenceIndex();
    mpc::sequencer::UpdateSequenceTracks updateSequenceTracks{sequenceIndex};
    stateManager.trackStatesSnapshots[sequenceIndex] =
        mpc::sequencer::SequenceTrackStatesSnapshot();
    updateSequenceTracks.trackStates =
        &stateManager.trackStatesSnapshots[sequenceIndex];

    mpc::sequencer::UpdateSequenceEvents updateSequenceEvents{sequenceIndex};
    updateSequenceEvents.trackSnapshots =
        &stateManager.trackEventsSnapshots[sequenceIndex];
    updateSequenceEvents.trackSnapshots->clear();

    if (parsed.parsedEvents != nullptr)
    {
        loadLegacyEventsFromParsedObjects(
            parsed, sequence, *updateSequenceEvents.trackSnapshots,
            *updateSequenceTracks.trackStates, sequencer);
    }
    else
    {
        loadLegacyEventsFromRawBytes(
            parsed, bytes, sequence, *updateSequenceEvents.trackSnapshots,
            *updateSequenceTracks.trackStates, sequencer);
    }

    for (const auto &headerPtr : *parsed.trackHeaders)
    {
        if (!headerPtr || headerPtr->absolute_recorded_track_number() == -1)
        {
            continue;
        }

        const auto trackIndex = mpc::TrackIndex(std::clamp(
            static_cast<int>(headerPtr->user_track_number()) - 1, 0,
            mpc::Mpc2000XlSpecs::TRACK_COUNT - 1));
        applyLegacyTrackHeader(
            *headerPtr, (*updateSequenceTracks.trackStates)[trackIndex],
            sequencer.getDefaultTrackName(trackIndex));
    }

    stateManager.enqueue(updateSequenceTracks);
    stateManager.enqueue(updateSequenceEvents);
}

void loadLegacyMpc3000AllSequenceIntoSlot(
    mpc::Mpc &mpc,
    const std::vector<char> &seqBytes,
    const mpc::SequenceIndex destinationIndex)
{
    std::istringstream kaitaiStream(
        std::string(seqBytes.begin(), seqBytes.end()),
        std::ios::in | std::ios::binary);
    ::kaitai::kstream kaitaiIo(&kaitaiStream);
    mpc3000_seq_v3_t parsed(&kaitaiIo);
    parsed._read();

    auto sequencer = mpc.getSequencer();
    applyLegacyParsedSequenceToInMemorySequence(
        makeLegacyParsedSequenceView(parsed), seqBytes,
        sequencer->getSequence(destinationIndex), *sequencer,
        *sequencer->getStateManager());
}

void loadLegacyMpc60AllSequenceIntoSlot(
    mpc::Mpc &mpc,
    const mpc60_seq_body_t::sequence_t &sourceSequence,
    const mpc::SequenceIndex destinationIndex)
{
    auto *mutableSequence =
        const_cast<mpc60_seq_body_t::sequence_t *>(&sourceSequence);
    auto sequencer = mpc.getSequencer();
    applyLegacyParsedSequenceToInMemorySequence(
        makeLegacyParsedSequenceView(*mutableSequence), {},
        sequencer->getSequence(destinationIndex), *sequencer,
        *sequencer->getStateManager());
}

mpc::sequencer::EventData
parsedEventToEventData(const mpc2000xl_all_t::event_t& parsedEvent)
{
    mpc::sequencer::EventData e;
    e.tick = parsedEvent.tick();
    e.trackIndex = mpc::TrackIndex(parsedEvent.track());

    if (parsedEvent.note_event() != nullptr)
    {
        e.type = mpc::sequencer::EventType::NoteOn;
        e.noteNumber = mpc::NoteNumber(parsedEvent.note_event()->note());
        e.duration = mpc::Duration(parsedEvent.note_event()->duration());
        e.velocity = mpc::Velocity(parsedEvent.note_event()->velocity());
        e.noteVariationType =
            mpc::NoteVariationType(parsedEvent.note_event()->variation_type());
        e.noteVariationValue =
            mpc::NoteVariationValue(parsedEvent.note_event()->variation_value());
        return e;
    }

    if (parsedEvent.pitch_bend() != nullptr)
    {
        e.type = mpc::sequencer::EventType::PitchBend;
        auto candidate = static_cast<int>(
            parsedEvent.pitch_bend()->amount_bits_1() +
            (parsedEvent.pitch_bend()->amount_bits_2() << 8)
        ) - 16384;
        if (candidate < -8192)
        {
            candidate += 8192;
        }
        e.amount = candidate;
        return e;
    }

    if (parsedEvent.control_change() != nullptr)
    {
        e.type = mpc::sequencer::EventType::ControlChange;
        e.controllerNumber = parsedEvent.control_change()->controller();
        e.controllerValue = parsedEvent.control_change()->value();
        return e;
    }

    if (parsedEvent.program_change() != nullptr)
    {
        e.type = mpc::sequencer::EventType::ProgramChange;
        e.programChangeProgramIndex =
            mpc::ProgramIndex(parsedEvent.program_change()->program());
        return e;
    }

    if (parsedEvent.ch_pressure() != nullptr)
    {
        e.type = mpc::sequencer::EventType::ChannelPressure;
        e.amount = parsedEvent.ch_pressure()->pressure();
        return e;
    }

    if (parsedEvent.poly_pressure() != nullptr)
    {
        e.type = mpc::sequencer::EventType::PolyPressure;
        e.noteNumber = mpc::NoteNumber(parsedEvent.poly_pressure()->note());
        e.amount = parsedEvent.poly_pressure()->pressure();
        return e;
    }

    if (parsedEvent.exclusive() != nullptr)
    {
        if (parsedEvent.exclusive()->mixer() != nullptr)
        {
            e.type = mpc::sequencer::EventType::Mixer;
            auto mixerParameter =
                static_cast<int>(parsedEvent.exclusive()->mixer()->param()) - 1;
            if (mixerParameter == 4)
            {
                mixerParameter = 3;
            }
            e.mixerParameter = mixerParameter;
            e.mixerPad = parsedEvent.exclusive()->mixer()->pad_index();
            e.mixerValue = parsedEvent.exclusive()->mixer()->value();
        }
        else
        {
            e.type = mpc::sequencer::EventType::SystemExclusive;
            const auto& bytes = parsedEvent.exclusive()->bytes();
            e.sysExByteA = bytes.size() > 0
                ? static_cast<uint8_t>(bytes[0])
                : 0;
            e.sysExByteB = bytes.size() > 1
                ? static_cast<uint8_t>(bytes[1])
                : 0;
        }
    }

    return e;
}

void applyParsedSequenceToInMemorySequence(
    mpc2000xl_all_t& parsed,
    const size_t sourceIndex,
    const std::shared_ptr<mpc::sequencer::Sequence>& inMemorySequence,
    mpc::sequencer::SequencerStateManager* const manager)
{
    auto& parsedSequence = *parsed.sequences()->at(sourceIndex);
    auto* body = parsedSequence.body();
    if (body == nullptr)
    {
        return;
    }

    const auto tempoBytes = body->_unnamed2();
    const auto tempo = static_cast<double>(mpc::file::ByteUtil::bytes2ushort({tempoBytes[3], tempoBytes[4]})) / 10.0;

    inMemorySequence->init(body->bar_count() - 1);

    for (size_t i = 0; i < body->bars()->size(); ++i)
    {
        inMemorySequence->setTimeSignature(
            static_cast<int>(i),
            body->bars()->at(i)->numerator(),
            body->bars()->at(i)->denominator()
        );
    }

    const auto slotIndex = static_cast<size_t>(body->index() - 1);
    const auto* sequenceMeta = parsed.sequences_metas()->at(slotIndex).get();
    inMemorySequence->setName(mpc::StrUtil::trim(bytesUntilNull(sequenceMeta->name())));
    inMemorySequence->setInitialTempo(tempo);

    const auto sequenceIndex = inMemorySequence->getSequenceIndex();

    mpc::sequencer::UpdateSequenceTracks updateSequenceTracks{sequenceIndex};
    manager->trackStatesSnapshots[sequenceIndex] = mpc::sequencer::SequenceTrackStatesSnapshot();
    updateSequenceTracks.trackStates = &manager->trackStatesSnapshots[sequenceIndex];

    auto& trackStates = *updateSequenceTracks.trackStates;
    const auto* parsedTracks = body->tracks();

    for (int i = 0; i < mpc::Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        const auto* status = parsedTracks->status()->at(i).get();
        trackStates[i].name = bytesUntilNull(parsedTracks->names()->at(i));
        trackStates[i].deviceIndex = parsedTracks->device()->at(i);
        trackStates[i].busType = mpc::sequencer::busIndexToBusType(static_cast<int>(parsedTracks->bus()->at(i)));
        trackStates[i].programChange = parsedTracks->program_change()->at(i);
        trackStates[i].on = status->off_or_on() == mpc2000xl_all_t::OFF_ON_TRUE;
        trackStates[i].velocityRatio = parsedTracks->velocity_ratio()->at(i);
        trackStates[i].transmitProgramChangesEnabled =
            status->transmit_program_changes() == mpc2000xl_all_t::OFF_ON_TRUE;
        trackStates[i].used = status->unused_or_used() == mpc2000xl_all_t::UNUSED_USED_USED;
    }

    manager->enqueue(updateSequenceTracks);

    mpc::sequencer::UpdateSequenceEvents updateSequenceEvents{sequenceIndex};
    updateSequenceEvents.trackSnapshots = &manager->trackEventsSnapshots[sequenceIndex];
    updateSequenceEvents.trackSnapshots->clear();

    for (const auto& parsedEvent : *body->events())
    {
        if (parsedEvent->tick() >= 0xFFFFF)
        {
            continue;
        }

        auto e = parsedEventToEventData(*parsedEvent);
        if (e.type == mpc::sequencer::EventType::Unknown)
        {
            continue;
        }

        (*updateSequenceEvents.trackSnapshots)[e.trackIndex].push_back(e);
    }

    manager->enqueue(updateSequenceEvents);

    for (int i = 0; i < 32; i++)
    {
        inMemorySequence->setDeviceName(i, bytesUntilNull(body->device_names()->at(i + 1)));
    }

    inMemorySequence->setFirstLoopBarIndex(mpc::BarIndex(body->loop_start_bar_index()));
    if (body->loop_end_bar_index() > 998)
    {
        inMemorySequence->setLastLoopBarIndex(mpc::EndOfSequence);
    }
    else
    {
        inMemorySequence->setLastLoopBarIndex(mpc::BarIndex(body->loop_end_bar_index()));
    }

    inMemorySequence->setLoopEnabled(body->loop_enabled());
    inMemorySequence->getStartTime().hours = body->start_time()->hours();
    inMemorySequence->getStartTime().minutes = body->start_time()->minutes();
    inMemorySequence->getStartTime().seconds = body->start_time()->seconds();
    inMemorySequence->getStartTime().frames = body->start_time()->frames();
    inMemorySequence->getStartTime().frameDecimals = body->start_time()->frame_decimals();
}

size_t embeddedMpc3000SequenceByteCount(
    const mpc3000_all_v3_t::sequence_t& sequence)
{
    const auto* misc = sequence.misc_chunks();
    return kLegacyAllEmbeddedPreludeSize +
        (static_cast<size_t>(misc->num_track_headers()) *
            kMpc3000TrackHeaderSize) +
        (static_cast<size_t>(misc->num_tempo_changes()) *
            kMpc3000TempoChangeSize) +
        static_cast<size_t>(
            misc->sequence_header()->event_stream_length_in_bytes()->value());
}

std::vector<char> buildStandaloneMpc3000SequenceBytes(
    const std::vector<char>& allBytes,
    const size_t sequenceOffset,
    const mpc3000_all_v3_t::sequence_t& sequence)
{
    const auto embeddedByteCount = embeddedMpc3000SequenceByteCount(sequence);
    const auto sequenceEnd = sequenceOffset + embeddedByteCount;
    if (sequenceEnd > allBytes.size())
    {
        throw std::runtime_error("MPC3000 ALL sequence extends past end of file");
    }

    std::vector<char> result;
    result.reserve(2 + embeddedByteCount);
    result.push_back(0x03);
    result.push_back(0x03);
    result.insert(
        result.end(),
        allBytes.begin() + static_cast<std::ptrdiff_t>(sequenceOffset),
        allBytes.begin() + static_cast<std::ptrdiff_t>(sequenceEnd)
    );
    return result;
}

void loadEverythingFromMpc3000AllBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_all_v3_t parsed(&parseIo);
    parsed._read();

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->deleteAllSequences();
    for (int i = 0; i < 20; ++i)
    {
        sequencer->getSong(i)->setUsed(false);
    }

    size_t sequenceOffset = kLegacyAllHeaderSize;
    for (size_t i = 0; i < parsed.sequences()->size(); ++i)
    {
        const auto seqBytes = buildStandaloneMpc3000SequenceBytes(
            bytes,
            sequenceOffset,
            *parsed.sequences()->at(i)
        );
        sequenceOffset += embeddedMpc3000SequenceByteCount(*parsed.sequences()->at(i));

        loadLegacyMpc3000AllSequenceIntoSlot(
            mpc, seqBytes, mpc::SequenceIndex(static_cast<int>(i)));
    }

    for (const auto& parsedSongPtr : *parsed.songs())
    {
        const auto* body = parsedSongPtr->song_body();
        if (body == nullptr)
        {
            continue;
        }

        const auto targetIndex = std::clamp(
            static_cast<int>(body->song_number()) - 1,
            0,
            19
        );
        const auto song = sequencer->getSong(targetIndex);
        song->setUsed(true);
        song->setName(mpc::StrUtil::trim(body->song_name()));

        for (size_t stepIndex = 0; stepIndex < body->steps()->size(); ++stepIndex)
        {
            const auto* parsedStep = body->steps()->at(stepIndex).get();
            song->insertStep(mpc::SongStepIndex(static_cast<int>(stepIndex)));
            song->setStepSequenceIndex(
                mpc::SongStepIndex(static_cast<int>(stepIndex)),
                mpc::SequenceIndex(std::max(0, static_cast<int>(parsedStep->sequence_number()) - 1))
            );
            song->setStepRepetitionCount(
                mpc::SongStepIndex(static_cast<int>(stepIndex)),
                parsedStep->repetition_count()
            );
        }

        const auto loopEnabled =
            body->end_status() ==
            mpc3000_all_v3_t::song_t::song_body_t::END_STATUS_LOOP_TO_A_STEP;
        song->setLoopEnabled(loopEnabled);
        song->setFirstLoopStepIndex(
            mpc::SongStepIndex(std::max(0, static_cast<int>(body->loop_back_step_number()) - 1))
        );
        song->setLastLoopStepIndex(
            mpc::SongStepIndex(std::max(0, static_cast<int>(body->steps()->size()) - 1))
        );
    }

    if (!parsed.sequences()->empty())
    {
        sequencer->setSelectedSequenceIndex(mpc::MinSequenceIndex, false);
        sequencer->setSelectedTrackIndex(0);
        sequencer->getTransport()->setPosition(0);
    }
}

void loadEverythingFromMpc60AllBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc60_all_v2_t parsed(&parseIo);
    parsed._read();

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->deleteAllSequences();
    for (int i = 0; i < 20; ++i)
    {
        sequencer->getSong(i)->setUsed(false);
    }

    for (size_t i = 0; i < parsed.body()->sequences()->size(); ++i)
    {
        const auto& sequence = *parsed.body()->sequences()->at(i);
        loadLegacyMpc60AllSequenceIntoSlot(
            mpc, sequence, mpc::SequenceIndex(static_cast<int>(i)));
    }

    for (const auto& parsedSongPtr : *parsed.body()->songs())
    {
        auto* songBody =
            dynamic_cast<mpc60_all_body_t::song_body_t*>(parsedSongPtr->body());
        if (songBody == nullptr)
        {
            continue;
        }

        const auto targetIndex = std::clamp(
            static_cast<int>(songBody->song_number()) - 1,
            0,
            19
        );
        const auto song = sequencer->getSong(targetIndex);
        song->setUsed(true);
        song->setName(mpc::StrUtil::trim(songBody->song_name()));

        for (size_t stepIndex = 0; stepIndex < songBody->steps()->size(); ++stepIndex)
        {
            const auto* parsedStep = songBody->steps()->at(stepIndex).get();
            const auto songStepIndex = mpc::SongStepIndex(static_cast<int>(stepIndex));
            song->insertStep(songStepIndex);
            song->setStepSequenceIndex(
                songStepIndex,
                mpc::SequenceIndex(std::max(
                    0, static_cast<int>(parsedStep->sequence_number()) - 1))
            );
            song->setStepRepetitionCount(songStepIndex, parsedStep->repeats());
        }

        song->setFirstLoopStepIndex(mpc::SongStepIndex(0));
        song->setLastLoopStepIndex(
            mpc::SongStepIndex(std::max(
                0, static_cast<int>(songBody->steps()->size()) - 1))
        );
        song->setLoopEnabled(false);
    }

    if (!parsed.body()->sequences()->empty())
    {
        sequencer->setSelectedSequenceIndex(mpc::MinSequenceIndex, false);
        sequencer->setSelectedTrackIndex(0);
        sequencer->getTransport()->setPosition(0);
    }
}

std::vector<mpc::sequencer::SequenceMetaInfo> loadMpc3000SequenceMetaInfos(
    const std::vector<char>& bytes)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_all_v3_t parsed(&parseIo);
    parsed._read();

    std::vector<mpc::sequencer::SequenceMetaInfo> result;
    result.reserve(parsed.sequences()->size());

    for (const auto& sequence : *parsed.sequences())
    {
        result.push_back(mpc::sequencer::SequenceMetaInfo{
            true,
            mpc::StrUtil::trim(sequence->misc_chunks()->sequence_header()->sequence_name())
        });
    }

    return result;
}

std::vector<mpc::sequencer::SequenceMetaInfo> loadMpc60SequenceMetaInfos(
    const std::vector<char>& bytes)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc60_all_v2_t parsed(&parseIo);
    parsed._read();

    std::vector<mpc::sequencer::SequenceMetaInfo> result;
    result.reserve(parsed.body()->sequences()->size());

    for (const auto& sequence : *parsed.body()->sequences())
    {
        result.push_back(mpc::sequencer::SequenceMetaInfo{
            true,
            mpc::StrUtil::trim(sequence->sequence_header()->sequence_name())
        });
    }

    return result;
}

std::shared_ptr<mpc::sequencer::Sequence> loadOneSequenceFromMpc3000AllBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes,
    const mpc::SequenceIndex sourceIndexInAllFile,
    const mpc::SequenceIndex destIndexInMpcMemory)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_all_v3_t parsed(&parseIo);
    parsed._read();

    const auto sourceIndex = static_cast<size_t>(sourceIndexInAllFile);
    const auto destination = mpc.getSequencer()->getSequence(destIndexInMpcMemory);
    if (sourceIndex >= parsed.sequences()->size())
    {
        return destination;
    }

    size_t sequenceOffset = kLegacyAllHeaderSize;
    for (size_t i = 0; i < sourceIndex; ++i)
    {
        sequenceOffset += embeddedMpc3000SequenceByteCount(*parsed.sequences()->at(i));
    }

    const auto seqBytes = buildStandaloneMpc3000SequenceBytes(
        bytes,
        sequenceOffset,
        *parsed.sequences()->at(sourceIndex)
    );
    loadLegacyMpc3000AllSequenceIntoSlot(mpc, seqBytes, destIndexInMpcMemory);
    return destination;
}

std::shared_ptr<mpc::sequencer::Sequence> loadOneSequenceFromMpc60AllBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& bytes,
    const mpc::SequenceIndex sourceIndexInAllFile,
    const mpc::SequenceIndex destIndexInMpcMemory)
{
    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc60_all_v2_t parsed(&parseIo);
    parsed._read();

    const auto sourceIndex = static_cast<size_t>(sourceIndexInAllFile);
    const auto destination = mpc.getSequencer()->getSequence(destIndexInMpcMemory);
    if (sourceIndex >= parsed.body()->sequences()->size())
    {
        return destination;
    }

    const auto& sequence = *parsed.body()->sequences()->at(sourceIndex);
    loadLegacyMpc60AllSequenceIntoSlot(mpc, sequence, destIndexInMpcMemory);
    return destination;
}

}

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

void AllLoader::loadEverythingFromFile(Mpc &mpc, MpcFile *f)
{
    loadEverythingFromBytes(mpc, f->getBytes());
}

void AllLoader::loadEverythingFromBytes(Mpc &mpc, const std::vector<char> &bytes)
{
    switch (detectAllVariant(bytes))
    {
        case all_variant_t::mpc2000xl:
        {
            const auto canonicalBytes = file::kaitai::parseRewrite<mpc2000xl_all_t>(bytes);
            loadEverythingFromCanonicalBytes(mpc, canonicalBytes);
            return;
        }
        case all_variant_t::mpc60_v2:
            loadEverythingFromMpc60AllBytes(mpc, bytes);
            return;
        case all_variant_t::mpc3000_v3:
            loadEverythingFromMpc3000AllBytes(mpc, bytes);
            return;
        default:
            throw std::runtime_error("Unsupported ALL file format");
    }
}

void AllLoader::loadEverythingFromCanonicalBytes(
    Mpc &mpc,
    const std::vector<char> &canonicalBytes)
{
    std::stringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    auto mpcSequencer = mpc.getSequencer();

    auto* defaults = parsed.defaults();
    auto* parsedSequencer = parsed.sequencer();
    auto* songGlobal = parsed.song_global();

    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();

    userScreen->setLastBar(static_cast<int>(defaults->bar_count()) - 1);
    userScreen->setLoop(defaults->unknown2()[4] != 0x00);
    userScreen->setBus(mpc::sequencer::busIndexToBusType(static_cast<int>(defaults->buses()->at(0))));

    mpcSequencer->setDefaultSequenceName(bytesUntilNull(defaults->sequence_name()));
    for (int i = 0; i < mpc::Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        mpcSequencer->setDefaultTrackName(i, bytesUntilNull(defaults->track_names()->at(i)));
    }

    userScreen->setDeviceNumber(defaults->devices()->at(0));
    userScreen->setTimeSig(defaults->numerator(), defaults->denominator());
    userScreen->setPgm(defaults->programs()->at(0));
    userScreen->setTempo(defaults->tempo() * 0.1);
    userScreen->setVelo(defaults->track_velocities()->at(0));

    mpcSequencer->deleteAllSequences();

    for (size_t sourceIndex = 0; sourceIndex < parsed.sequences()->size(); ++sourceIndex)
    {
        if (parsed.sequences()->at(sourceIndex)->body() == nullptr)
        {
            continue;
        }

        const auto targetIndex = static_cast<int>(parsed.sequences()->at(sourceIndex)->body()->index()) - 1;
        auto mpcSeq = mpcSequencer->getSequence(targetIndex);
        applyParsedSequenceToInMemorySequence(
            parsed,
            sourceIndex,
            mpcSeq,
            mpcSequencer->getStateManager().get()
        );
    }

    mpcSequencer->setSelectedSequenceIndex(
        SequenceIndex(parsedSequencer->active_sequence()), false);
    mpcSequencer->getTransport()->setPosition(0);
    mpcSequencer->setSelectedTrackIndex(parsedSequencer->active_track());
    mpcSequencer->getTransport()->setMasterTempo(
        parsedSequencer->master_tempo() * 0.1);
    mpcSequencer->getTransport()->setTempoSourceIsSequence(
        parsedSequencer->tempo_source_is_sequence());

    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(static_cast<int>(parsedSequencer->timing_correct()));

    auto timeDisplayScreen = mpc.screens->get<ScreenId::TimeDisplayScreen>();
    timeDisplayScreen->setDisplayStyle(
        static_cast<int>(parsedSequencer->time_display_style()));

    auto* count = parsed.count();
    auto* midiOutput = parsed.midi_output();
    auto* midiInput = parsed.midi_input();
    auto* midiSync = parsed.midi_sync();
    auto* misc = parsed.misc();
    auto* stepEditOptions = parsed.step_edit_options();

    auto countMetronomeScreen =
        mpc.screens->get<ScreenId::CountMetronomeScreen>();
    auto metronomeSoundScreen =
        mpc.screens->get<ScreenId::MetronomeSoundScreen>();

    countMetronomeScreen->setCountIn(static_cast<int>(count->count_in_mode()));
    metronomeSoundScreen->accentPad = count->accent_pad_index();
    metronomeSoundScreen->normalPad = count->normal_pad_index();
    metronomeSoundScreen->setAccentVelo(count->accent_velo());
    metronomeSoundScreen->setNormalVelo(count->normal_velo());
    metronomeSoundScreen->setOutput(static_cast<int>(count->click_output()));
    metronomeSoundScreen->setVolume(count->click_volume());
    countMetronomeScreen->setRate(static_cast<int>(count->rate()));
    metronomeSoundScreen->setSound(static_cast<int>(count->sound_source()));
    countMetronomeScreen->setInPlay(count->enabled_in_play());
    countMetronomeScreen->setInRec(count->enabled_in_rec());
    countMetronomeScreen->setWaitForKey(count->wait_for_key());
    mpcSequencer->getTransport()->setCountEnabled(count->enabled());

    mpc.screens->get<ScreenId::MidiOutputScreen>()->setSoftThru(
        static_cast<int>(midiOutput->soft_thru_mode()));

    auto midiInputScreen = mpc.screens->get<ScreenId::MidiInputScreen>();

    midiInputScreen->setReceiveCh(
        static_cast<int>(midiInput->receive_channel()) - 1);
    midiInputScreen->setType(static_cast<int>(midiInput->filter_type()));
    midiInputScreen->setMidiFilterEnabled(midiInput->filter_enabled());
    midiInputScreen->setSustainPedalToDuration(
        midiInput->sustain_pedal_to_duration());

    auto multiRecordingSetupScreen =
        mpc.screens->get<ScreenId::MultiRecordingSetupScreen>();

    for (int i = 0; i < midiInput->multi_rec_destination_tracks()->size(); i++)
    {
        multiRecordingSetupScreen->getMrsLines()[i]->setTrack(
            midiInput->multi_rec_destination_tracks()->at(i) - 1);
    }

    midiInputScreen->setChPressurePassEnabled(
        midiInput->ch_pressure_pass_enabled());
    midiInputScreen->setExclusivePassEnabled(
        midiInput->exclusive_pass_enabled());
    mpcSequencer->setRecordingModeMulti(midiInput->multi_rec_enabled());
    midiInputScreen->setNotePassEnabled(midiInput->note_pass_enabled());
    midiInputScreen->setPgmChangePassEnabled(
        midiInput->pgm_change_pass_enabled());
    midiInputScreen->setPitchBendPassEnabled(
        midiInput->pitch_bend_pass_enabled());
    midiInputScreen->setPolyPressurePassEnabled(
        midiInput->poly_pressure_pass_enabled());
    midiInputScreen->getCcPassEnabled() = *midiInput->cc_pass_enabled();

    auto stepEditOptionsScreen =
        mpc.screens->get<ScreenId::StepEditOptionsScreen>();
    stepEditOptionsScreen->setAutoStepIncrementEnabled(
        stepEditOptions->auto_step_increment());
    stepEditOptionsScreen->setDurationOfRecordedNotesTcValue(
        stepEditOptions->duration_of_recorded_notes() ==
        mpc2000xl_all_t::DURATION_OF_RECORDED_NOTES_TC_VALUE);
    stepEditOptionsScreen->setTcValueRecordedNotes(
        stepEditOptions->tc_value_percentage());
    midiInputScreen->setProgChangeSeq(parsed.prog_change_to_seq());

    auto locateScreen = mpc.screens->get<ScreenId::LocateScreen>();
    std::vector<LocateScreen::Location> locations;
    locations.reserve(parsed.locations()->size());
    for (const auto& location : *parsed.locations())
    {
        locations.emplace_back(location->bar(), location->beat(), location->clock());
    }
    locateScreen->setLocations(locations);

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    using MT = input::midi::MidiBindingBase::MessageType;

    int bindingCounter = 0;
    for (const auto& midiSwitch : *misc->midi_switch())
    {
        const int cc = midiSwitch->controller() == 0xFF ? -1 : midiSwitch->controller();
        const auto functionIndex = static_cast<int>(midiSwitch->function());
        auto fn =
            static_cast<controller::MidiFootswitchFunction>(functionIndex);

        if (auto hw = controller::footswitchToComponentId.find(fn);
            hw != controller::footswitchToComponentId.end())
        {
            footswitchController->bindings[bindingCounter++] =
                input::midi::HardwareBinding{{-1, cc, std::nullopt,
                                              input::midi::Interaction::Press,
                                              MT::CC},
                                             {hw->second}};
        }
        else if (auto sq = controller::footswitchToSequencerCmd.find(fn);
                 sq != controller::footswitchToSequencerCmd.end())
        {
            footswitchController->bindings[bindingCounter++] =
                input::midi::SequencerBinding{{-1, cc, std::nullopt,
                                               input::midi::Interaction::Press,
                                               MT::CC},
                                              {sq->second}};
        }
        else
        {
            std::cout << "[Footswitch] Unmapped function index: "
                      << functionIndex << "\n";
        }
    }

    auto othersScreen = mpc.screens->get<ScreenId::OthersScreen>();

    othersScreen->setTapAveraging(static_cast<int>(misc->tap_averaging()) + 2);

    auto syncScreen = mpc.screens->get<ScreenId::SyncScreen>();

    syncScreen->receiveMMCEnabled = misc->midi_sync_in_receive_mmc_enabled();
    syncScreen->sendMMCEnabled = midiSync->send_mmc_enabled();
    syncScreen->in = midiSync->input();
    syncScreen->out = static_cast<int>(midiSync->output());

    if (!mpc.isPluginModeEnabled())
    {
        syncScreen->setModeIn(static_cast<int>(midiSync->in_mode()));
        syncScreen->setModeOut(static_cast<int>(midiSync->out_mode()));
    }

    syncScreen->shiftEarly = midiSync->shift_early();
    syncScreen->frameRate = static_cast<int>(midiSync->frame_rate());

    mpcSequencer->setSecondSequenceEnabled(
        parsedSequencer->second_sequence_enabled());

    auto secondSequenceScreen = mpc.screens->get<ScreenId::SecondSeqScreen>();
    secondSequenceScreen->sq = parsedSequencer->sequence_sequence_index();

    auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    songScreen->setDefaultSongName(songGlobal->default_song_name());
    auto ignoreTempoChangeScreen =
        mpc.screens->get<ScreenId::IgnoreTempoChangeScreen>();
    ignoreTempoChangeScreen->setIgnore(
        songGlobal->ignore_tempo_change_events_in_sequence());

    const auto* songs = parsed.songs();

    for (int i = 0; i < 20; i++)
    {
        const auto& parsedSong = songs->at(i);
        auto mpcSong = mpcSequencer->getSong(i);
        mpcSong->setUsed(false);

        const auto trimmedName = mpc::StrUtil::trim(bytesUntilNull(parsedSong->name()));
        const bool isUsed = parsedSong->is_used() || trimmedName != "(Unused)";
        if (isUsed)
        {
            mpcSong->setUsed(true);
            mpcSong->setName(trimmedName);

            int stepIndex = 0;
            for (const auto& step : *parsedSong->steps())
            {
                if (step->sequence_index() == 0xFF || step->repeat_count() == 0xFF)
                {
                    continue;
                }

                const auto idx = SongStepIndex(stepIndex++);
                mpcSong->insertStep(idx);
                mpcSong->setStepSequenceIndex(
                    idx, SequenceIndex(step->sequence_index()));
                mpcSong->setStepRepetitionCount(idx, step->repeat_count());
            }

            mpcSong->setFirstLoopStepIndex(
                SongStepIndex(parsedSong->loop_first_step()));

            mpcSong->setLastLoopStepIndex(
                SongStepIndex(parsedSong->loop_last_step()));

            mpcSong->setLoopEnabled(parsedSong->is_loop_enabled());
        }
    }

    mpcSequencer->getStateManager()->enqueue(
        SetSelectedSongStepIndex{SongStepIndex(0), NoSequenceIndex});
}

std::vector<SequenceMetaInfo>
AllLoader::loadSequenceMetaInfosFromFile(Mpc &mpc, MpcFile *f)
{
    const auto bytes = f->getBytes();
    switch (detectAllVariant(bytes))
    {
        case all_variant_t::mpc2000xl:
        {
            auto result = file::kaitai::AllIo::loadSequenceMetaInfos(mpc, f);
            if (!result)
            {
                throw std::runtime_error(result.error());
            }
            return *result;
        }
        case all_variant_t::mpc60_v2:
            return loadMpc60SequenceMetaInfos(bytes);
        case all_variant_t::mpc3000_v3:
            return loadMpc3000SequenceMetaInfos(bytes);
        default:
            throw std::runtime_error("Unsupported ALL file format");
    }
}

std::shared_ptr<Sequence>
AllLoader::loadOneSequenceFromCanonicalBytes(
    Mpc &mpc,
    const std::vector<char> &canonicalBytes,
    const SequenceIndex sourceIndexInAllFile,
    const SequenceIndex destIndexInMpcMemory)
{
    std::stringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    size_t sourceOrdinal = parsed.sequences()->size();
    const auto sourceSlot = static_cast<int>(sourceIndexInAllFile);
    for (size_t i = 0; i < parsed.sequences()->size(); ++i)
    {
        auto* body = parsed.sequences()->at(i)->body();
        if (body == nullptr)
        {
            continue;
        }

        if (static_cast<int>(body->index()) - 1 == sourceSlot)
        {
            sourceOrdinal = i;
            break;
        }
    }

    const auto sequence = mpc.getSequencer()->getSequence(destIndexInMpcMemory);
    if (sourceOrdinal == parsed.sequences()->size())
    {
        return sequence;
    }

    applyParsedSequenceToInMemorySequence(
        parsed,
        sourceOrdinal,
        sequence,
        mpc.getSequencer()->getStateManager().get()
    );

    return sequence;
}

std::shared_ptr<Sequence>
AllLoader::loadOneSequenceFromFile(Mpc &mpc, MpcFile *f,
                                   const SequenceIndex sourceIndexInAllFile,
                                   const SequenceIndex destIndexInMpcMemory)
{
    const auto bytes = f->getBytes();
    switch (detectAllVariant(bytes))
    {
        case all_variant_t::mpc2000xl:
            return file::kaitai::AllIo::loadOneSequence(
                mpc, f, sourceIndexInAllFile, destIndexInMpcMemory);
        case all_variant_t::mpc60_v2:
            return loadOneSequenceFromMpc60AllBytes(
                mpc, bytes, sourceIndexInAllFile, destIndexInMpcMemory);
        case all_variant_t::mpc3000_v3:
            return loadOneSequenceFromMpc3000AllBytes(
                mpc, bytes, sourceIndexInAllFile, destIndexInMpcMemory);
        default:
            throw std::runtime_error("Unsupported ALL file format");
    }
}
