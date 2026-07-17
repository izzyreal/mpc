#include "file/kaitai/MidIo.hpp"

#include "Mpc.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/standard_midi_file_with_running_status.h"
#include "sequencer/BusType.hpp"
#include "sequencer/EventData.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <sstream>

using namespace mpc::file::kaitai;

namespace {

using midi_t = standard_midi_file_with_running_status_t;
using meta_type_t = midi_t::meta_event_body_t::meta_type_enum_t;

std::string asciiBody(const std::string& body)
{
    return body;
}

bool isInteger(const std::string& s)
{
    for (const auto c : s)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }
    return true;
}

bool isMpc2000xlTrackName(const std::string& value)
{
    return value.rfind("MPC2000XL 1.00  ", 0) == 0;
}

bool isMpc2000xlMetaTrack(const midi_t& parsed)
{
    if (parsed.tracks() == nullptr || parsed.tracks()->empty())
    {
        return false;
    }

    for (const auto& eventPtr : *parsed.tracks()->at(0)->events()->event())
    {
        const auto* meta = eventPtr->meta_event_body();
        if (meta == nullptr ||
            meta->meta_type() != meta_type_t::META_TYPE_ENUM_SEQUENCE_TRACK_NAME)
        {
            continue;
        }

        if (isMpc2000xlTrackName(asciiBody(meta->body())))
        {
            return true;
        }
    }

    return false;
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

struct TimedMidiEvent
{
    int tick;
    int order;
    std::vector<char> bytes;
};

void appendU2Be(std::vector<char>& out, const uint16_t value)
{
    out.push_back(static_cast<char>((value >> 8) & 0xFF));
    out.push_back(static_cast<char>(value & 0xFF));
}

void appendU4Be(std::vector<char>& out, const uint32_t value)
{
    out.push_back(static_cast<char>((value >> 24) & 0xFF));
    out.push_back(static_cast<char>((value >> 16) & 0xFF));
    out.push_back(static_cast<char>((value >> 8) & 0xFF));
    out.push_back(static_cast<char>(value & 0xFF));
}

void appendVlq(std::vector<char>& out, uint32_t value)
{
    std::array<char, 5> reversed{};
    auto count = 0;
    reversed[count++] = static_cast<char>(value & 0x7F);
    value >>= 7;

    while (value > 0)
    {
        reversed[count++] = static_cast<char>((value & 0x7F) | 0x80);
        value >>= 7;
    }

    for (auto i = count - 1; i >= 0; --i)
    {
        out.push_back(reversed[i]);
    }
}

std::vector<char> metaEventBytes(const uint8_t type, const std::vector<char>& payload)
{
    std::vector<char> result{
        static_cast<char>(0xFF),
        static_cast<char>(type)
    };
    appendVlq(result, static_cast<uint32_t>(payload.size()));
    result.insert(result.end(), payload.begin(), payload.end());
    return result;
}

std::vector<char> channelEventBytes(const uint8_t status, const uint8_t dataA)
{
    return std::vector<char>{
        static_cast<char>(status),
        static_cast<char>(dataA)
    };
}

std::vector<char> channelEventBytes(const uint8_t status, const uint8_t dataA, const uint8_t dataB)
{
    return std::vector<char>{
        static_cast<char>(status),
        static_cast<char>(dataA),
        static_cast<char>(dataB)
    };
}

std::vector<char> sysexEventBytes(const std::vector<char>& payload)
{
    std::vector<char> result{
        static_cast<char>(0xF0)
    };
    appendVlq(result, static_cast<uint32_t>(payload.size()));
    result.insert(result.end(), payload.begin(), payload.end());
    return result;
}

std::string zeroPad3(const int value)
{
    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << std::clamp(value, 0, 999);
    return ss.str();
}

std::string padRightTruncate(const std::string& value, const size_t width)
{
    if (value.size() >= width)
    {
        return value.substr(0, width);
    }
    return value + std::string(width - value.size(), ' ');
}

std::string hexByte(const int value)
{
    std::ostringstream ss;
    ss << std::uppercase << std::hex
       << std::setw(2) << std::setfill('0')
       << (value & 0xFF);
    return ss.str();
}

bool isDefaultNoteVariation(const mpc::sequencer::EventData& event)
{
    return event.noteVariationType == mpc::NoteVariationTypeTune &&
           event.noteVariationValue == mpc::DefaultNoteVariationValue;
}

std::string sequenceParamsText(const mpc::sequencer::Sequence& sequence)
{
    const auto loopStart = zeroPad3(sequence.getFirstLoopBarIndex());
    const auto loopEnd =
        sequence.getLastLoopBarIndex() == mpc::EndOfSequence
            ? std::string("END")
            : zeroPad3(sequence.getLastLoopBarIndex());

    return std::string("LOOP=") +
           (sequence.isLoopEnabled() ? "ON " : "OFF") +
           " START=" + loopStart +
           " END=" + loopEnd +
           " TEMPO=" + (sequence.isTempoChangeOn() ? "ON " : "OFF") + " ";
}

std::vector<char> stringPayload(const std::string& value)
{
    return std::vector<char>(value.begin(), value.end());
}

int tempoToMpqn(const double bpm)
{
    const auto clamped = std::clamp(bpm, 30.0, 300.0);
    return static_cast<int>(60000000.0 / clamped);
}

int timeSignatureDenominatorPow(const int denominator)
{
    auto value = denominator;
    auto power = 0;
    while (value > 1)
    {
        value >>= 1;
        power++;
    }
    return power;
}

std::vector<char> serializeTrack(const std::vector<TimedMidiEvent>& inputEvents, const int lastTick)
{
    auto events = inputEvents;
    std::stable_sort(
        events.begin(),
        events.end(),
        [](const auto& a, const auto& b)
        {
            if (a.tick != b.tick)
            {
                return a.tick < b.tick;
            }
            return a.order < b.order;
        });

    std::vector<char> trackBody;
    auto previousTick = 0;
    for (const auto& event : events)
    {
        appendVlq(trackBody, static_cast<uint32_t>(std::max(0, event.tick - previousTick)));
        trackBody.insert(trackBody.end(), event.bytes.begin(), event.bytes.end());
        previousTick = event.tick;
    }

    appendVlq(trackBody, static_cast<uint32_t>(std::max(0, lastTick - previousTick)));
    trackBody.push_back(static_cast<char>(0xFF));
    trackBody.push_back(static_cast<char>(0x2F));
    trackBody.push_back(static_cast<char>(0x00));

    std::vector<char> result{'M', 'T', 'r', 'k'};
    appendU4Be(result, static_cast<uint32_t>(trackBody.size()));
    result.insert(result.end(), trackBody.begin(), trackBody.end());
    return result;
}

std::vector<char> buildMetaTrack(const mpc::sequencer::Sequence& sequence)
{
    std::vector<TimedMidiEvent> events;
    events.push_back(TimedMidiEvent{
        0,
        0,
        metaEventBytes(0x01, stringPayload(sequenceParamsText(sequence)))
    });
    events.push_back(TimedMidiEvent{
        0,
        1,
        metaEventBytes(
            0x03,
            stringPayload("MPC2000XL 1.00  " + padRightTruncate(sequence.getName(), 16)))
    });

    const auto initialMpqn = tempoToMpqn(sequence.getInitialTempo());
    events.push_back(TimedMidiEvent{
        0,
        2,
        metaEventBytes(
            0x51,
            std::vector<char>{
                static_cast<char>((initialMpqn >> 16) & 0xFF),
                static_cast<char>((initialMpqn >> 8) & 0xFF),
                static_cast<char>(initialMpqn & 0xFF)})
    });

    for (const auto& tempoChange : sequence.getTempoChangeEvents())
    {
        if (tempoChange->getTick() <= 0 || tempoChange->getTick() >= sequence.getLastTick())
        {
            continue;
        }

        const auto mpqn = tempoToMpqn(tempoChange->getTempo());
        events.push_back(TimedMidiEvent{
            tempoChange->getTick(),
            2,
            metaEventBytes(
                0x51,
                std::vector<char>{
                    static_cast<char>((mpqn >> 16) & 0xFF),
                    static_cast<char>((mpqn >> 8) & 0xFF),
                    static_cast<char>(mpqn & 0xFF)})
        });
    }

    events.push_back(TimedMidiEvent{
        0,
        3,
        metaEventBytes(0x54, std::vector<char>{0, 0, 0, 0, 0})
    });

    auto barStartTick = 0;
    auto previousNumerator = sequence.getNumerator(0);
    auto previousDenominator = sequence.getDenominator(0);
    events.push_back(TimedMidiEvent{
        0,
        4,
        metaEventBytes(
            0x58,
            std::vector<char>{
                static_cast<char>(previousNumerator),
                static_cast<char>(timeSignatureDenominatorPow(previousDenominator)),
                static_cast<char>(24),
                static_cast<char>(8)})
    });

    for (auto barIndex = 1; barIndex <= sequence.getLastBarIndex(); ++barIndex)
    {
        barStartTick += sequence.getBarLength(barIndex - 1);
        const auto numerator = sequence.getNumerator(barIndex);
        const auto denominator = sequence.getDenominator(barIndex);
        if (numerator == previousNumerator && denominator == previousDenominator)
        {
            continue;
        }

        previousNumerator = numerator;
        previousDenominator = denominator;
        events.push_back(TimedMidiEvent{
            barStartTick,
            4,
            metaEventBytes(
                0x58,
                std::vector<char>{
                    static_cast<char>(numerator),
                    static_cast<char>(timeSignatureDenominatorPow(denominator)),
                    static_cast<char>(24),
                    static_cast<char>(8)})
        });
    }

    return serializeTrack(events, sequence.getLastTick());
}

std::vector<char> buildSequenceTrack(const std::shared_ptr<mpc::sequencer::Sequence>& sequence,
                                     const std::shared_ptr<mpc::sequencer::Track>& track)
{
    std::vector<TimedMidiEvent> events;
    events.push_back(TimedMidiEvent{
        0,
        0,
        metaEventBytes(0x04, stringPayload(std::string(8, ' ')))
    });

    const auto deviceIndex = track->getDeviceIndex();
    const auto trackNumber = zeroPad3(track->getIndex()).substr(1, 2);
    const auto trackDevice =
        deviceIndex == 0 ? std::string("C0") : hexByte(0xE0 + deviceIndex - 1);
    const auto deviceNumber = hexByte(deviceIndex);
    const auto busIndex = hexByte(static_cast<int>(mpc::sequencer::busTypeToIndex(track->getBusType())));
    events.push_back(TimedMidiEvent{
        0,
        1,
        metaEventBytes(
            0x01,
            stringPayload(
                "TRACK DATA:" +
                trackNumber +
                trackDevice +
                "006403  " +
                deviceNumber +
                busIndex +
                deviceNumber +
                "   "))
    });
    events.push_back(TimedMidiEvent{
        0,
        2,
        metaEventBytes(0x03, stringPayload(padRightTruncate(track->getName(), 16)))
    });

    std::vector<TimedMidiEvent> noteOffEvents;
    std::vector<TimedMidiEvent> noteVariationEvents;
    std::vector<TimedMidiEvent> noteOnEvents;
    std::vector<TimedMidiEvent> miscEvents;

    for (const auto& event : track->getEventStates())
    {
        if (event.tick >= sequence->getLastTick())
        {
            continue;
        }

        switch (event.type)
        {
            case mpc::sequencer::EventType::NoteOn:
            {
                const auto noteOffTick = static_cast<int>(event.tick + event.duration);
                for (auto& noteOffEvent : noteOffEvents)
                {
                    if (noteOffEvent.tick > event.tick &&
                        noteOffEvent.bytes.size() >= 3 &&
                        static_cast<unsigned char>(noteOffEvent.bytes[1]) ==
                            static_cast<unsigned char>(event.noteNumber))
                    {
                        noteOffEvent.tick = event.tick;
                    }
                }

                if (!isDefaultNoteVariation(event))
                {
                    noteVariationEvents.push_back(TimedMidiEvent{
                        static_cast<int>(event.tick),
                        1,
                        channelEventBytes(
                            0x80,
                            static_cast<uint8_t>(event.noteVariationType.get()),
                            static_cast<uint8_t>(event.noteVariationValue.get()))
                    });
                }

                noteOnEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    2,
                    channelEventBytes(
                        0x90,
                        static_cast<uint8_t>(event.noteNumber.get()),
                        static_cast<uint8_t>(event.velocity.get()))
                });

                if (noteOffTick < sequence->getLastTick())
                {
                    noteOffEvents.push_back(TimedMidiEvent{
                        noteOffTick,
                        0,
                        channelEventBytes(
                            0x90,
                            static_cast<uint8_t>(event.noteNumber.get()),
                            0)
                    });
                }
                break;
            }
            case mpc::sequencer::EventType::PolyPressure:
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    channelEventBytes(
                        0xA0,
                        static_cast<uint8_t>(event.noteNumber.get()),
                        static_cast<uint8_t>(event.amount & 0x7F))
                });
                break;
            case mpc::sequencer::EventType::ControlChange:
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    channelEventBytes(
                        0xB0,
                        static_cast<uint8_t>(event.controllerNumber & 0x7F),
                        static_cast<uint8_t>(event.controllerValue & 0x7F))
                });
                break;
            case mpc::sequencer::EventType::ProgramChange:
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    channelEventBytes(
                        0xC0,
                        static_cast<uint8_t>(event.programChangeProgramIndex.get()))
                });
                break;
            case mpc::sequencer::EventType::ChannelPressure:
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    channelEventBytes(
                        0xD0,
                        static_cast<uint8_t>(event.amount & 0x7F))
                });
                break;
            case mpc::sequencer::EventType::PitchBend:
            {
                const auto bendValue = std::clamp<int>(event.amount + 8192, 0, 16383);
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    channelEventBytes(
                        0xE0,
                        static_cast<uint8_t>(bendValue & 0x7F),
                        static_cast<uint8_t>((bendValue >> 7) & 0x7F))
                });
                break;
            }
            case mpc::sequencer::EventType::Mixer:
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    sysexEventBytes(std::vector<char>{
                        71,
                        0,
                        68,
                        69,
                        static_cast<char>(event.mixerParameter + 1),
                        static_cast<char>(event.mixerPad),
                        static_cast<char>(event.mixerValue),
                        static_cast<char>(0xF7)})
                });
                break;
            case mpc::sequencer::EventType::SystemExclusive:
                miscEvents.push_back(TimedMidiEvent{
                    static_cast<int>(event.tick),
                    3,
                    sysexEventBytes(std::vector<char>{
                        static_cast<char>(event.sysExByteA),
                        static_cast<char>(event.sysExByteB)})
                });
                break;
            default:
                break;
        }
    }

    events.insert(events.end(), noteOffEvents.begin(), noteOffEvents.end());
    events.insert(events.end(), noteVariationEvents.begin(), noteVariationEvents.end());
    events.insert(events.end(), noteOnEvents.begin(), noteOnEvents.end());
    events.insert(events.end(), miscEvents.begin(), miscEvents.end());
    return serializeTrack(events, sequence->getLastTick());
}

std::vector<char> buildMpc2000xlMidiBytes(const std::shared_ptr<mpc::sequencer::Sequence>& sequence)
{
    auto firstUsedTrackIndex = -1;
    auto lastUsedTrackIndex = -1;
    for (auto i = 0; i < 64; ++i)
    {
        if (!sequence->getTrack(i)->isUsed())
        {
            continue;
        }
        if (firstUsedTrackIndex == -1)
        {
            firstUsedTrackIndex = i;
        }
        lastUsedTrackIndex = i;
    }

    std::vector<std::vector<char>> tracks;
    tracks.push_back(buildMetaTrack(*sequence));

    for (auto trackIndex = firstUsedTrackIndex;
         trackIndex >= 0 && trackIndex <= lastUsedTrackIndex;
         ++trackIndex)
    {
        tracks.push_back(buildSequenceTrack(sequence, sequence->getTrack(trackIndex)));
    }

    std::vector<char> result{'M', 'T', 'h', 'd'};
    appendU4Be(result, 6);
    appendU2Be(result, tracks.size() > 1 ? 1 : 0);
    appendU2Be(result, static_cast<uint16_t>(tracks.size()));
    appendU2Be(result, 96);

    for (const auto& track : tracks)
    {
        result.insert(result.end(), track.begin(), track.end());
    }
    return result;
}

bool loadGenericStandardMidi(mpc::Mpc& mpc,
                             const midi_t& parsed,
                             const std::shared_ptr<mpc::sequencer::Sequence>& sequence)
{
    auto sequencer = mpc.getSequencer();
    auto transport = sequencer->getTransport();
    auto stateManager = sequencer->getStateManager();

    sequence->setUsed(true);

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

    const auto sequenceIndex = sequence->getSequenceIndex();
    mpc::sequencer::UpdateSequenceEvents updateSequenceEvents{sequenceIndex};
    updateSequenceEvents.trackSnapshots = &stateManager->trackEventsSnapshots[sequenceIndex];
    updateSequenceEvents.trackSnapshots->clear();

    mpc::sequencer::UpdateSequenceTracks updateSequenceTracks{sequenceIndex};
    updateSequenceTracks.trackStates = &stateManager->trackStatesSnapshots[sequenceIndex];
    *updateSequenceTracks.trackStates = mpc::sequencer::SequenceTrackStatesSnapshot();

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

        auto& trackState = (*updateSequenceTracks.trackStates)[resolvedTrackIndex];
        trackState.busType = busType;
        trackState.deviceIndex = deviceIndex;
        std::string pendingTrackName;
        bool hasPendingTrackName = false;
        bool hasImportedEvents = false;

        struct OpenNote
        {
            mpc::sequencer::EventData event;
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
                    (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
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
                    (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
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
                    (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
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
                    (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
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
                    (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
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
                        (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
                    }
                    else
                    {
                        mpc::sequencer::EventData e;
                        e.type = mpc::sequencer::EventType::SystemExclusive;
                        e.tick = absoluteTick;
                        (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(e);
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

            (*updateSequenceEvents.trackSnapshots)[resolvedTrackIndex].push_back(on.event);
            hasImportedEvents = true;
        }

        if (hasPendingTrackName)
        {
            trackState.name = pendingTrackName;
        }

        trackState.used = hasImportedEvents;
    }

    stateManager->enqueue(updateSequenceTracks);
    stateManager->enqueue(updateSequenceEvents);
    stateManager->drainQueue();

    return true;
}

bool tryLoadMpc2000xl(mpc::Mpc& mpc,
                      const midi_t& parsed,
                      const std::shared_ptr<mpc::sequencer::Sequence>& sequence)
{
    if (!isMpc2000xlMetaTrack(parsed))
    {
        return false;
    }

    auto sequencer = mpc.getSequencer();
    auto transport = sequencer->getTransport();

    sequence->setUsed(true);

    struct TimedMetaEvent
    {
        int tick;
        const midi_t::track_event_t* event;
        const midi_t::meta_event_body_t* meta;
    };

    std::vector<TimedMetaEvent> metaTrackEvents;
    int tick = 0;
    for (const auto& eventPtr : *parsed.tracks()->at(0)->events()->event())
    {
        tick += eventPtr->v_time()->value();
        if (const auto* meta = eventPtr->meta_event_body(); meta != nullptr)
        {
            metaTrackEvents.push_back(TimedMetaEvent{tick, eventPtr.get(), meta});
        }
    }

    std::vector<std::pair<int, double>> tempoChanges;
    struct TimeSigChange { int tick; int numerator; int denominator; };
    std::vector<TimeSigChange> timeSignatures;
    int firstLoopBar = -1;
    int lastLoopBar = -1;

    for (const auto& entry : metaTrackEvents)
    {
        const auto metaType = entry.meta->meta_type();
        const auto body = asciiBody(entry.meta->body());

        if (metaType == meta_type_t::META_TYPE_ENUM_TEXT_EVENT)
        {
            if (body.find("LOOP=ON") != std::string::npos)
            {
                sequence->setLoopEnabled(true);
            }
            else if (body.find("LOOP=OFF") != std::string::npos)
            {
                sequence->setLoopEnabled(false);
            }

            if (body.find("TEMPO=ON") != std::string::npos)
            {
                transport->setTempoSourceIsSequence(true);
            }
            else if (body.find("TEMPO=OFF") != std::string::npos)
            {
                transport->setTempoSourceIsSequence(false);
            }

            if (body.size() >= 18)
            {
                firstLoopBar = std::stoi(body.substr(15, 18));
            }
            lastLoopBar = -1;
            if (body.size() >= 26)
            {
                const auto loopEnd = body.substr(23, 26);
                if (isInteger(loopEnd))
                {
                    lastLoopBar = std::stoi(loopEnd);
                }
            }
        }
        else if (metaType == meta_type_t::META_TYPE_ENUM_SEQUENCE_TRACK_NAME &&
                 isMpc2000xlTrackName(body))
        {
            sequence->setName(body.substr(16));
        }
        else if (metaType == meta_type_t::META_TYPE_ENUM_TEMPO)
        {
            tempoChanges.push_back({entry.tick, bpmFromTempoBody(entry.meta->body())});
        }
        else if (metaType == meta_type_t::META_TYPE_ENUM_TIME_SIGNATURE)
        {
            timeSignatures.push_back(
                TimeSigChange{
                    entry.tick,
                    entry.meta->body().empty()
                        ? mpc::DefaultTimeSigNumerator
                        : static_cast<unsigned char>(entry.meta->body()[0]),
                    denominatorFromTimeSignatureBody(entry.meta->body())});
        }
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
        sequence->addTempoChangeEvent(tempoChanges[i].first,
                                      static_cast<int>(ratio * 1000.0));
    }

    const bool hadExplicitTimeSignatures = !timeSignatures.empty();
    if (!hadExplicitTimeSignatures)
    {
        timeSignatures.push_back(
            TimeSigChange{0, mpc::DefaultTimeSigNumerator, mpc::DefaultTimeSigDenominator});
    }

    int lengthInTicks = 1;
    for (const auto& trackPtr : *parsed.tracks())
    {
        int trackTick = 0;
        for (const auto& eventPtr : *trackPtr->events()->event())
        {
            trackTick += eventPtr->v_time()->value();
        }
        lengthInTicks = std::max(lengthInTicks, trackTick);
    }

    int accumLength = 0;
    int barCounter = 0;
    for (size_t i = 0; i < timeSignatures.size(); ++i)
    {
        const auto current = timeSignatures[i];
        const auto hasNext = i + 1 < timeSignatures.size();
        const auto nextTick = hasNext ? timeSignatures[i + 1].tick : lengthInTicks;

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
    sequence->setFirstLoopBarIndex(mpc::BarIndex(firstLoopBar));
    if (lastLoopBar == -1)
    {
        sequence->setLastLoopBarIndex(mpc::EndOfSequence);
    }
    else
    {
        sequence->setLastLoopBarIndex(mpc::BarIndex(lastLoopBar));
    }

    for (size_t trackContainerIndex = 1; trackContainerIndex < parsed.tracks()->size(); ++trackContainerIndex)
    {
        const auto& midiTrack = parsed.tracks()->at(trackContainerIndex);
        auto trackIndex = static_cast<int>(trackContainerIndex - 1);
        auto deviceIndex = 0;
        auto busType = mpc::sequencer::BusType::DRUM1;
        auto track = sequence->getTrack(trackIndex);

        struct OpenNote
        {
            mpc::sequencer::EventData event;
            bool isClosed = false;
        };

        auto noteVariationType = mpc::NoteVariationTypeTune;
        auto noteVariationValue = mpc::DefaultNoteVariationValue;
        std::vector<OpenNote> openNotes;
        std::string pendingTrackName;
        bool hasPendingTrackName = false;
        bool hasImportedEvents = false;
        int absoluteTick = 0;

        for (const auto& eventPtr : *midiTrack->events()->event())
        {
            absoluteTick += eventPtr->v_time()->value();

            if (const auto* meta = eventPtr->meta_event_body(); meta != nullptr)
            {
                const auto body = asciiBody(meta->body());
                if (meta->meta_type() == meta_type_t::META_TYPE_ENUM_TEXT_EVENT &&
                    body.find("TRACK DATA:") != std::string::npos)
                {
                    const auto payload = body.substr(std::string("TRACK DATA:").length());
                    trackIndex = std::stoi(payload.substr(0, 2));
                    if (payload.substr(2, 2) != "C0")
                    {
                        deviceIndex =
                            std::stoi(payload.substr(2, 2), nullptr, 16) -
                            std::stoi(std::string("E0"), nullptr, 16) + 1;
                    }
                    if (payload.size() >= 16)
                    {
                        busType = mpc::sequencer::busIndexToBusType(
                            std::stoi(payload.substr(14, 2), nullptr, 16));
                    }
                    track = sequence->getTrack(trackIndex);
                    track->setBusType(busType, false);
                    track->setDeviceIndex(deviceIndex, false);
                }
                else if (meta->meta_type() ==
                             meta_type_t::META_TYPE_ENUM_SEQUENCE_TRACK_NAME &&
                         !body.empty())
                {
                    pendingTrackName =
                        body.substr(0, mpc::Mpc2000XlSpecs::MAX_TRACK_NAME_LENGTH);
                    hasPendingTrackName = true;
                }
                continue;
            }

            switch (eventPtr->event_type())
            {
                case 0x80:
                {
                    const auto* noteOff =
                        dynamic_cast<midi_t::note_off_event_t*>(eventPtr->event_body());
                    if (noteOff == nullptr)
                    {
                        break;
                    }
                    noteVariationType =
                        mpc::NoteVariationType(noteOff->note());
                    noteVariationValue =
                        mpc::NoteVariationValue(noteOff->velocity());
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

                    if (noteOn->velocity() == 0)
                    {
                        auto bestIt = openNotes.end();
                        const auto isDefaultVariation =
                            [](const mpc::sequencer::EventData& event)
                            {
                                if (event.noteVariationType == mpc::NoteVariationTypeTune)
                                {
                                    return event.noteVariationValue ==
                                           mpc::DefaultNoteVariationValue;
                                }
                                if (event.noteVariationType == mpc::NoteVariationTypeDecay ||
                                    event.noteVariationType == mpc::NoteVariationTypeAttack)
                                {
                                    return event.noteVariationValue == 0;
                                }
                                if (event.noteVariationType == mpc::NoteVariationTypeFilter)
                                {
                                    return event.noteVariationValue == 50;
                                }
                                return false;
                            };
                        const auto isEligible =
                            [&](const OpenNote& openNote)
                            {
                                return !openNote.isClosed &&
                                       openNote.event.noteNumber ==
                                           mpc::NoteNumber(noteOn->note()) &&
                                       openNote.event.tick <= absoluteTick;
                            };

                        auto latestNonDefaultIt = openNotes.end();
                        for (auto it = openNotes.rbegin(); it != openNotes.rend(); ++it)
                        {
                            if (!isEligible(*it) || isDefaultVariation(it->event))
                            {
                                continue;
                            }
                            latestNonDefaultIt = std::prev(it.base());
                            break;
                        }

                        if (latestNonDefaultIt != openNotes.end())
                        {
                            auto sameVariationCount = 0;
                            for (auto it = openNotes.begin(); it != openNotes.end(); ++it)
                            {
                                if (!isEligible(*it))
                                {
                                    continue;
                                }
                                if (it->event.noteVariationType ==
                                        latestNonDefaultIt->event.noteVariationType &&
                                    it->event.noteVariationValue ==
                                        latestNonDefaultIt->event.noteVariationValue)
                                {
                                    sameVariationCount++;
                                    if (bestIt == openNotes.end())
                                    {
                                        bestIt = it;
                                    }
                                }
                            }

                            if (sameVariationCount < 2)
                            {
                                bestIt = openNotes.end();
                            }
                        }

                        if (bestIt == openNotes.end())
                        {
                            for (auto it = openNotes.begin(); it != openNotes.end(); ++it)
                            {
                                if (!isEligible(*it))
                                {
                                    continue;
                                }
                                bestIt = it;
                                break;
                            }
                        }

                        if (bestIt != openNotes.end())
                        {
                            bestIt->event.duration =
                                mpc::Duration(absoluteTick - bestIt->event.tick);
                            bestIt->isClosed = true;
                        }
                    }
                    else
                    {
                        mpc::sequencer::EventData e;
                        e.type = mpc::sequencer::EventType::NoteOn;
                        e.noteNumber = mpc::NoteNumber(noteOn->note());
                        e.tick = absoluteTick;
                        e.velocity = mpc::Velocity(noteOn->velocity());
                        e.noteVariationType = noteVariationType;
                        e.noteVariationValue = noteVariationValue;
                        e.duration = mpc::Duration(24);
                        openNotes.push_back(OpenNote{e});
                        noteVariationType = mpc::NoteVariationTypeTune;
                        noteVariationValue = mpc::DefaultNoteVariationValue;
                    }
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
                    e.programChangeProgramIndex =
                        mpc::ProgramIndex(programChange->program());
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
                    else if (data.size() >= 2)
                    {
                        mpc::sequencer::EventData e;
                        e.type = mpc::sequencer::EventType::SystemExclusive;
                        e.tick = absoluteTick;
                        e.sysExByteA = static_cast<unsigned char>(data[0]);
                        e.sysExByteB = static_cast<unsigned char>(data[1]);
                        track->acquireAndInsertEvent(e);
                    }
                    hasImportedEvents = true;
                    break;
                }
            }
        }

        for (const auto& openNote : openNotes)
        {
            track->acquireAndInsertEvent(openNote.event);
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

    return true;
}
} // namespace

sequence_or_error MidIo::loadBytes(mpc::Mpc &mpc,
                                   const std::vector<char> &bytes,
                                   const std::string &fileNameWithoutExtension)
{
    const auto canonicalBytes = parseRewrite<standard_midi_file_with_running_status_t>(bytes);

    auto newSeq = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    newSeq->init(0);
    mpc.getSequencer()->getStateManager()->drainQueue();

    std::istringstream kaitaiStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::binary
    );
    ::kaitai::kstream kaitaiIo(&kaitaiStream);
    midi_t parsed(&kaitaiIo);
    parsed._read();

    if (!tryLoadMpc2000xl(mpc, parsed, newSeq))
    {
        loadGenericStandardMidi(mpc, parsed, newSeq);
    }

    if (newSeq->getName().empty())
    {
        newSeq->setName(fileNameWithoutExtension);
    }

    return newSeq;
}

sequence_or_error MidIo::load(mpc::Mpc &mpc, const std::shared_ptr<mpc::disk::MpcFile> &file)
{
    return loadBytes(mpc, file->getBytes(), file->getNameWithoutExtension());
}

std::vector<char> MidIo::saveBytes(const std::shared_ptr<mpc::sequencer::Sequence> &sequence)
{
    return parseRewrite<standard_midi_file_with_running_status_t>(
        buildMpc2000xlMidiBytes(sequence));
}

void MidIo::save(const std::shared_ptr<mpc::sequencer::Sequence> &sequence,
                 const std::shared_ptr<std::ostream> &outputStream)
{
    const auto bytes = saveBytes(sequence);
    outputStream->write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}
