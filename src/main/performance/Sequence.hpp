#pragma once

#include "MpcSpecs.hpp"
#include "IntTypes.hpp"

#include <array>

namespace mpc::performance
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

    struct Event
    {
        EventType type;
        SequenceIndex sequenceIndex;
        TrackIndex trackIndex;

        EventIndex eventIndex;
        EventIndex previousEventIndex;
        EventIndex nextEventIndex;

        Tick tick;

        NoteNumber noteNumber;
        NoteEventId noteEventId;
        bool beingRecorded;
        int wasMoved;
        bool dontDelete;
        int64_t metronomeOnlyTickPosition;

        Duration duration;
        Velocity velocity;
        NoteVariationType noteVariationType;
        NoteVariationValue noteVariationValue;

        int8_t amount;

        int8_t controllerNumber;
        int8_t controllerValue;

        ProgramIndex programChangeProgramIndex;

        int8_t mixerParameter;
        int8_t mixerPad;
        int8_t mixerValue;

        Event()
        {
            resetToDefaultValues();
        }

        void resetToDefaultValues()
        {
            type = EventType::Unknown;
            sequenceIndex = NoSequenceIndex;
            trackIndex = NoTrackIndex;

            eventIndex = NoEventIndex;
            previousEventIndex = NoEventIndex;
            nextEventIndex = NoEventIndex;

            tick = NoTick;

            noteNumber = NoNoteNumber;
            noteEventId = NoNoteEventId;
            beingRecorded = false;

            duration = NoDuration;
            velocity = NoVelocityOrPressure;
            noteVariationType = NoteVariationTypeTune;
            noteVariationValue = DefaultNoteVariationValue;
        }
    };

    struct EventPool
    {
        std::array<Event, 300'000> events{};
        int64_t nextEventIndex = 0;
    };

    struct Track
    {
        EventIndex eventIndex;
    };

    struct TimeSignature
    {
        TimeSigNumerator numerator{DefaultTimeSigNumerator};
        TimeSigDenominator denominator{DefaultTimeSigDenominator};
    };

    struct Sequence
    {
        std::array<Track, Mpc2000XlSpecs::TRACK_COUNT> tracks;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
    };
}