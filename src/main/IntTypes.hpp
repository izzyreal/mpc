#pragma once

#include "ConstrainedInt.hpp"

#include <cstdint>
#include <limits>

namespace mpc
{
    using PhysicalPadIndex = ConstrainedInt<uint8_t, 0, 15>;
    using Velocity = ConstrainedInt<int8_t, -1, 127>;
    using ProgramPadIndex = ConstrainedInt<uint8_t, 0, 63>;
    using MidiChannel = ConstrainedInt<int8_t, -1, 15>;
    using Pressure = ConstrainedInt<int8_t, -1, 127>;
    using NoteNumber = ConstrainedInt<int8_t, -1, 127>;
    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;
    using VelocityOrPressure = ConstrainedInt<int8_t, -1, 127>;
    using TrackIndex = ConstrainedInt<uint8_t, 0, 63>;
    using ProgramIndex = ConstrainedInt<int8_t, 0, 23>;
    using DrumBusIndex = ConstrainedInt<int8_t, 0, 3>;

    using NoteEventId = uint32_t;
    constexpr NoteEventId NoNoteEventId = 0;
    constexpr NoteEventId MinNoteEventId{1};
    constexpr NoteEventId MaxNoteEventId{
        std::numeric_limits<NoteEventId>::max()};

    inline NoteEventId getNextNoteEventId(NoteEventId current)
    {
        return current >= MaxNoteEventId ? MinNoteEventId : current + 1;
    }

    using TimeInMilliseconds = int64_t;
    using TimeInNanoseconds = int64_t;

    constexpr int8_t NoNoteNumber = -1;
    constexpr int8_t NoPressure = -1;
    constexpr int8_t NoVelocityOrPressure = -1;
    constexpr int8_t NoMidiChannel = -1;
    constexpr int8_t NoProgramIndex = -1;
} // namespace mpc
