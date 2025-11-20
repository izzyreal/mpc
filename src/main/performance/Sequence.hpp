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

        int64_t tick;

        int16_t duration;
        int8_t velocity;
        int8_t noteVarationType;
        int8_t noteVarationValue;

        Event()
        {
            resetToDefaultValues();
        }

        void resetToDefaultValues()
        {
            type = EventType::Unknown;
            trackIndex = NoTrackIndex;
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