#pragma once

#include "sequencer/EventState.hpp"

#include <cstdint>
#include <functional>
#include <variant>

namespace mpc::sequencer
{
    struct ClearEvents
    {
    };

    struct UpdateTrackIndexOfAllEvents
    {
        TrackIndex trackIndex;
    };

    struct FinalizeNonLiveNoteEvent
    {
        EventState noteOnEvent;
        Duration duration;
    };

    struct InsertEvent
    {
        EventState eventState;
        bool allowMultipleNoteEventsWithSameNoteOnSameTick;
        std::function<void()> onComplete;
    };

    struct RemoveEvent
    {
        EventId eventId;
    };

    struct UpdateEventTick
    {
        EventId eventId;
        Tick newTick;
    };

    struct RemoveDoubles
    {
    };

    struct UpdateEvent
    {
        EventState payload;
    };

    using TrackEventMessage =
        std::variant<InsertEvent, ClearEvents, RemoveEvent,
                     UpdateEventTick, RemoveDoubles,
                     UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent>;
} // namespace mpc::sequencer
