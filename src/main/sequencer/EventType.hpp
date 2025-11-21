#pragma once

#include <cstdint>

namespace mpc::sequencer
{
    enum class EventType : uint8_t {
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
        Unknown
    };
}