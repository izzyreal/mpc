#pragma once

#include "ConstrainedInt.hpp"
#include "MpcSpecs.hpp"

#include <cstdint>
#include <limits>

namespace mpc
{
    using PhysicalPadIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::HARDWARE_PAD_COUNT - 1>;
    using Velocity = ConstrainedInt<int8_t, -1, 127>;
    using ProgramPadIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::PROGRAM_PAD_COUNT - 1>;
    using MidiChannel = ConstrainedInt<int8_t, -1, 15>;
    using Pressure = ConstrainedInt<int8_t, -1, 127>;
    using NoteNumber = ConstrainedInt<int8_t, -1, 127>;
    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;
    using VelocityOrPressure = ConstrainedInt<int8_t, -1, 127>;
    using TrackIndex =
        ConstrainedInt<uint8_t, 0, Mpc2000XlSpecs::LAST_TRACK_INDEX + 1>;
    using ProgramIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX>;
    using DrumBusIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::DRUM_BUS_COUNT - 1>;
    using SequenceIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::LAST_SEQUENCE_INDEX>;

    using Value0To100 = ConstrainedInt<int8_t, 0, 100>;
    using DrumMixerLevel = Value0To100;
    using DrumMixerPanning = Value0To100;

    using DrumMixerIndividualOutput = ConstrainedInt<int8_t, 0, 8>;
    using DrumMixerIndividualFxPath = ConstrainedInt<int8_t, 0, 4>;

    constexpr DrumMixerLevel MinDrumMixerLevel{0};
    constexpr DrumMixerLevel MaxDrumMixerLevel{100};
    constexpr DrumMixerPanning PanningCenter{50};
    constexpr DrumMixerIndividualOutput MinDrumMixerIndividualOut{0};
    constexpr DrumMixerIndividualFxPath MinDrumMixerFxPath{0};

    using MidiNumber = ConstrainedInt<int8_t, 0, 127>;
    using MidiValue = ConstrainedInt<int8_t, -1, 127>;

    constexpr MidiValue MaxMidiValue{127};

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

    constexpr DrumBusIndex NoDrumBusIndex{-1};

    constexpr MidiValue NoMidiValue{-1};

    constexpr SequenceIndex NoSequenceIndex{-1};
    constexpr SequenceIndex MinSequenceIndex{0};
    constexpr SequenceIndex MaxSequenceIndex{
        Mpc2000XlSpecs::LAST_SEQUENCE_INDEX};

    constexpr PhysicalPadIndex NoPhysicalPadIndex{-1};

    constexpr ProgramPadIndex NoProgramPadIndex{-1};
    constexpr ProgramPadIndex MinProgramPadIndex{0};
    constexpr ProgramPadIndex MaxProgramPadIndex{
        Mpc2000XlSpecs::PROGRAM_PAD_COUNT - 1};

    constexpr NoteNumber NoNoteNumber{-1};
    constexpr NoteNumber MinNoteNumber{0};
    constexpr NoteNumber MaxNoteNumber{127};

    constexpr DrumNoteNumber NoDrumNoteAssigned{34};
    constexpr DrumNoteNumber MinDrumNoteNumber{Mpc2000XlSpecs::FIRST_DRUM_NOTE};
    constexpr DrumNoteNumber MaxDrumNoteNumber{Mpc2000XlSpecs::LAST_DRUM_NOTE};

    constexpr Pressure NoPressure{-1};

    constexpr VelocityOrPressure NoVelocityOrPressure{-1};

    constexpr Velocity MinVelocity{0};
    constexpr Velocity MaxVelocity{127};

    constexpr MidiChannel NoMidiChannel{-1};

    constexpr ProgramIndex NoProgramIndex{-1};
    constexpr ProgramIndex MinProgramIndex{0};
    constexpr ProgramIndex MaxProgramIndex{
        Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX};
} // namespace mpc
