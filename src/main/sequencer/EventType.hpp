#pragma once

#include <cstdint>
#include <string>

namespace mpc::sequencer
{
    enum class EventType : uint8_t
    {
        NoteOn = 0,
        NoteOff = 1,
        ProgramChange = 2,
        ControlChange = 3,
        PitchBend = 4,
        PolyPressure = 5,
        Mixer = 6,
        ChannelPressure = 7,
        SystemExclusive = 8,
        TempoChange = 9,
        MetronomeClick = 10,
        Unknown
    };

    inline std::string eventTypeToString(const EventType e)
    {
        switch (e)
        {
            case EventType::NoteOn:
                return "NoteOn";
            case EventType::NoteOff:
                return "NoteOff";
            case EventType::ProgramChange:
                return "ProgramChange";
            case EventType::ControlChange:
                return "ControlChange";
            case EventType::PitchBend:
                return "PitchBend";
            case EventType::ChannelPressure:
                return "ChannelPressure";
            case EventType::PolyPressure:
                return "PolyPressure";
            case EventType::Mixer:
                return "Mixer";
            case EventType::SystemExclusive:
                return "SystemExclusive";
            case EventType::TempoChange:
                return "TempoChange";
            case EventType::MetronomeClick:
                return "MetronomeClick";
            case EventType::Unknown:
            default:
                return "Unknown";
        }
    }
} // namespace mpc::sequencer