#include "file/kaitai/Mpc3000SeqIo.hpp"

#include "Mpc.hpp"
#include "MpcSpecs.hpp"
#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
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
using seq_t = mpc3000_seq_v3_t;
constexpr size_t kFixedPreludeSize =
    2 + 37 + 5 + (64 * 4) + 2 + 15 + 3 + 16 + 1 + 1 + 1;
constexpr size_t kTrackHeaderSize = 24;
constexpr size_t kTempoChangeSize = 6;

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
    const seq_t::track_header_t &header,
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
    const mpc3000_seq_v3_t &parsed,
    const std::vector<char> &bytes)
{
    const auto start = kFixedPreludeSize +
                       (parsed.number_of_active_track_headers() *
                        kTrackHeaderSize) +
                       (parsed.number_of_tempo_changes() * kTempoChangeSize);
    return std::min(start, bytes.size());
}

std::vector<TempoChangeData>
decodeTempoChanges(const seq_t &parsed, const double headerTempo)
{
    std::vector<TempoChangeData> result;
    const auto *tempoChanges = parsed.tempo_changes();
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
                headerTempo * (tempoChangePtr->factor_percentage() / 100.0)
            });
    }

    return result;
}
} // namespace

sequence_or_error Mpc3000SeqIo::loadBytes(
    mpc::Mpc &mpc,
    const std::vector<char> &bytes,
    const std::string &fileNameWithoutExtension)
{
    auto newSeq = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);

    std::stringstream kaitaiStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary);
    ::kaitai::kstream kaitaiIo(&kaitaiStream);
    seq_t parsed(&kaitaiIo);
    parsed._read();

    const auto barCount =
        std::max(1, static_cast<int>(parsed.sequence_header()->number_of_bars()));
    newSeq->init(barCount - 1);
    mpc.getSequencer()->getStateManager()->drainQueue();

    const auto headerTempo =
        static_cast<double>(parsed.sequence_header()->tempo()) / 10.0;
    const auto tempoChanges = decodeTempoChanges(parsed, headerTempo);

    auto initialTempo = headerTempo;
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

        const auto ratio =
            static_cast<int>(std::round((tempoChange.tempo / initialTempo) * 1000.0));
        newSeq->addTempoChangeEvent(tempoChange.tick, ratio);
    }

    const auto sequenceName = trimAscii(parsed.sequence_header()->sequence_name());
    newSeq->setName(sequenceName.empty() ? fileNameWithoutExtension : sequenceName);

    newSeq->setFirstLoopBarIndex(mpc::BarIndex(0));
    const auto loopEnabled =
        parsed.sequence_header()->loop_to_bar() == seq_t::OFF_ON_TRUE;
    newSeq->setLoopEnabled(loopEnabled);
    if (loopEnabled)
    {
        newSeq->setLastLoopBarIndex(mpc::BarIndex(std::max(
            0, static_cast<int>(parsed.sequence_header()->loop_to_bar_number()) - 1)));
    }
    else
    {
        newSeq->setLastLoopBarIndex(mpc::EndOfSequence);
    }

    std::vector<const seq_t::track_header_t *> trackHeaders(
        mpc::Mpc2000XlSpecs::TRACK_COUNT, nullptr);
    for (const auto &headerPtr : *parsed.track_headers())
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

    int absoluteTick = 0;
    size_t pos = eventStreamStartOffset(parsed, bytes);

    if (pos >= bytes.size())
    {
        return newSeq;
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
                const auto barIndex =
                    std::max(0, barNumber - 1);
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
                    e.mixerParameter =
                        static_cast<int8_t>(payload.at(4) - 1);
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

sequence_or_error
Mpc3000SeqIo::load(mpc::Mpc &mpc,
                   const std::shared_ptr<mpc::disk::MpcFile> &file)
{
    return loadBytes(mpc, file->getBytes(), file->getNameWithoutExtension());
}
