#pragma once

#include "ConstrainedInt.hpp"
#include "MpcSpecs.hpp"

#include <cstdint>
#include <limits>

namespace mpc
{
    using PhysicalPadIndex = ConstrainedInt<uint8_t, 0, Mpc2000XlSpecs::HARDWARE_PAD_COUNT - 1>;
    using Velocity = ConstrainedInt<int8_t, -1, 127>;
    using ProgramPadIndex = ConstrainedInt<uint8_t, 0, Mpc2000XlSpecs::PROGRAM_PAD_COUNT - 1>;
    using MidiChannel = ConstrainedInt<int8_t, -1, 15>;
    using Pressure = ConstrainedInt<int8_t, -1, 127>;
    using NoteNumber = ConstrainedInt<int8_t, -1, 127>;
    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;
    using VelocityOrPressure = ConstrainedInt<int8_t, -1, 127>;
    using TrackIndex = ConstrainedInt<uint8_t, 0, Mpc2000XlSpecs::LAST_TRACK_INDEX>;
    using ProgramIndex = ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX>;
    using DrumBusIndex = ConstrainedInt<int8_t, 0, Mpc2000XlSpecs::DRUM_BUS_COUNT - 1>;
    using SequenceIndex = ConstrainedInt<int8_t, 0, Mpc2000XlSpecs::LAST_SEQUENCE_INDEX>;

    using NoteEventId = uint32_t;
    constexpr NoteEventId NoNoteEventId = 0;
    constexpr NoteEventId MinNoteEventId{1};
    constexpr NoteEventId MaxNoteEventId{
        std::numeric_limits<NoteEventId>::max()};

    inline NoteEventId getNextNoteEventId(const NoteEventId current)
    {
        return current >= MaxNoteEventId ? MinNoteEventId : current + 1;
    }

    using TimeInMilliseconds = int64_t;
    using TimeInNanoseconds = int64_t;

    constexpr NoteNumber NoNoteNumber = -1;
    constexpr Pressure NoPressure = -1;
    constexpr VelocityOrPressure NoVelocityOrPressure = -1;
    constexpr MidiChannel NoMidiChannel = -1;
    constexpr ProgramIndex NoProgramIndex = -1;
} // namespace mpc
