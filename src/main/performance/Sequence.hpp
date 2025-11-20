#pragma once

#include "MpcSpecs.hpp"
#include "IntTypes.hpp"

#include <array>

namespace mpc::performance
{
    enum class EventType : uint8_t {
        Note = 0,
        ProgramChange = 1,
        ControlChange = 2,
        PitchBend = 3,
        PolyPressure = 4,
        Mixer = 5,
        Unknown
    };

    struct Event
    {
        EventType type;
        TrackIndex trackIndex;

        EventIndex eventIndex;
        EventIndex previousEventIndex;
        EventIndex nextEventIndex;

        Tick tick;

        Duration duration;
        Velocity velocity;
        NoteVariationType noteVarationType;
        NoteVariationValue noteVariationValue;

        Event()
        {
            resetToDefaultValues();
        }

        void resetToDefaultValues()
        {
            type = EventType::Unknown;
            trackIndex = NoTrackIndex;

            eventIndex = NoEventIndex;
            previousEventIndex = NoEventIndex;
            nextEventIndex = NoEventIndex;

            tick = NoTick;

            duration = NoDuration;
            velocity = NoVelocityOrPressure;
            noteVarationType = NoteVariationTypeTune;
            noteVariationValue = DefaultNoteVariationValue;
        }
    };

    struct EventPool
    {
        std::array<Event, 300'000> events{};
    };

    struct Track
    {
        EventIndex eventIndex;
    };

    struct Sequence
    {
        std::array<Track, Mpc2000XlSpecs::TRACK_COUNT> tracks;
    };
}