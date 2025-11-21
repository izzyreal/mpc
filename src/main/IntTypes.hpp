#pragma once

#include "ConstrainedInt.hpp"
#include "MpcSpecs.hpp"

#include <cstdint>
#include <limits>

namespace mpc
{
    ///////////

    using PhysicalPadIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::HARDWARE_PAD_COUNT - 1>;
    constexpr PhysicalPadIndex NoPhysicalPadIndex{-1};

    /////////

    using Velocity = ConstrainedInt<int8_t, -1, 127>;
    constexpr Velocity MinVelocity{0};
    constexpr Velocity Velocity1{1};
    constexpr Velocity MaxVelocity{127};

    /////////

    using ProgramPadIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::PROGRAM_PAD_COUNT - 1>;
    constexpr ProgramPadIndex NoProgramPadIndex{-1};
    constexpr ProgramPadIndex MinProgramPadIndex{0};
    constexpr ProgramPadIndex MaxProgramPadIndex{
        Mpc2000XlSpecs::PROGRAM_PAD_COUNT - 1};

    /////////

    using MidiChannel = ConstrainedInt<int8_t, -1, 15>;
    constexpr MidiChannel NoMidiChannel{-1};

    /////////

    using Pressure = ConstrainedInt<int8_t, -1, 127>;
    constexpr Pressure NoPressure{-1};

    /////////

    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;
    constexpr DrumNoteNumber NoDrumNoteAssigned{34};
    constexpr DrumNoteNumber MinDrumNoteNumber{Mpc2000XlSpecs::FIRST_DRUM_NOTE};
    constexpr DrumNoteNumber MaxDrumNoteNumber{Mpc2000XlSpecs::LAST_DRUM_NOTE};

    /////////

    using NoteNumber = ConstrainedInt<int8_t, -1, 127>;
    constexpr NoteNumber NoNoteNumber{-1};
    constexpr NoteNumber MinNoteNumber{0};
    constexpr NoteNumber MaxNoteNumber{127};

    inline bool isDrumNote(const NoteNumber number)
    {
        return number >= MinDrumNoteNumber && number <= MaxDrumNoteNumber;
    }

    /////////

    using VelocityOrPressure = ConstrainedInt<int8_t, -1, 127>;
    constexpr VelocityOrPressure NoVelocityOrPressure{-1};

    /////////

    using TrackIndex = ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::TRACK_COUNT>;
    constexpr TrackIndex NoTrackIndex{-1};

    /////////

    using ProgramIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX>;
    constexpr ProgramIndex NoProgramIndex{-1};
    constexpr ProgramIndex MinProgramIndex{0};
    constexpr ProgramIndex MaxProgramIndex{
        Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX};

    /////////

    using DrumBusIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::DRUM_BUS_COUNT - 1>;

    constexpr DrumBusIndex NoDrumBusIndex{-1};

    /////////

    using SequenceIndex =
        ConstrainedInt<int8_t, -1, Mpc2000XlSpecs::LAST_SEQUENCE_INDEX>;

    constexpr SequenceIndex NoSequenceIndex{-1};
    constexpr SequenceIndex MinSequenceIndex{0};
    constexpr SequenceIndex MaxSequenceIndex{
        Mpc2000XlSpecs::LAST_SEQUENCE_INDEX};

    /////////

    using EventIndex =
        ConstrainedInt<int64_t, -1, Mpc2000XlSpecs::GLOBAL_EVENT_CAPACITY>;

    constexpr EventIndex NoEventIndex{-1};

    /////////

    using Value0To100 = ConstrainedInt<int8_t, 0, 100>;

    /////////

    using DrumMixerLevel = Value0To100;
    constexpr DrumMixerLevel MinDrumMixerLevel{0};
    constexpr DrumMixerLevel MaxDrumMixerLevel{100};

    /////////

    using DrumMixerPanning = Value0To100;
    constexpr DrumMixerPanning PanningCenter{50};

    /////////

    using DrumMixerIndividualOutput = ConstrainedInt<int8_t, 0, 8>;
    constexpr DrumMixerIndividualOutput MinDrumMixerIndividualOut{0};

    /////////

    using DrumMixerIndividualFxPath = ConstrainedInt<int8_t, 0, 4>;
    constexpr DrumMixerIndividualFxPath MinDrumMixerFxPath{0};

    /////////

    using MidiNumber = ConstrainedInt<int8_t, 0, 127>;

    /////////

    using MidiValue = ConstrainedInt<int8_t, -1, 127>;
    constexpr MidiValue NoMidiValue{-1};
    constexpr MidiValue MaxMidiValue{127};

    /////////

    using NoteEventId = uint32_t;
    constexpr NoteEventId NoNoteEventId = 0;
    constexpr NoteEventId MinNoteEventId{1};
    constexpr NoteEventId MaxNoteEventId{
        std::numeric_limits<NoteEventId>::max()};

    inline NoteEventId getNextNoteEventId(const NoteEventId current)
    {
        return current >= MaxNoteEventId ? MinNoteEventId : current + 1;
    }

    /////////

    using Tick = int64_t;
    constexpr Tick NoTick{-1};
    constexpr Tick NoTickAssignedWhileRecording{-2};

    /////////

    using Duration = ConstrainedInt<int16_t, -1, 9999>;
    constexpr Duration NoDuration{-1};

    /////////

    using NoteVariationType = ConstrainedInt<int8_t, 0, 3>;
    constexpr NoteVariationType NoteVariationTypeTune{0};
    constexpr NoteVariationType NoteVariationTypeDecay{1};
    constexpr NoteVariationType NoteVariationTypeAttack{2};
    constexpr NoteVariationType NoteVariationTypeFilter{3};

    /////////

    using NoteVariationValue = ConstrainedInt<int8_t, 0, 124>;
    constexpr NoteVariationValue DefaultNoteVariationValue{64};

    /////////

    using TimeInMilliseconds = int64_t;
    using TimeInNanoseconds = int64_t;

    /////////

    using TimeSigNumerator = ConstrainedInt<int8_t, 0, 32>;
    using TimeSigDenominator = ConstrainedInt<int8_t, 0, 32>;

    constexpr TimeSigNumerator DefaultTimeSigNumerator{4};
    constexpr TimeSigDenominator DefaultTimeSigDenominator{4};

} // namespace mpc
