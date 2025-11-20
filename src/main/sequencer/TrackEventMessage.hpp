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
        EventIndex eventIndex;
    };

    struct RemoveEventByIndex
    {
        EventIndex eventIndex;
    };

    struct UpdateEventTick
    {
        EventIndex eventIndex;
        Tick newTick;
    };

    struct RemoveDoubles
    {
    };

    struct UpdateEvent
    {
        std::pair<EventIndex, EventState> payload;
    };

    using TrackEventMessage =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, RemoveEventByIndex,
                     UpdateEventTick, RemoveDoubles,
                     UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent>;
} // namespace mpc::sequencer
