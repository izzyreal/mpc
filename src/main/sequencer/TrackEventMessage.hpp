#pragma once

#include "sequencer/EventState.hpp"

#include <cstdint>
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

    struct SyncEventIndex
    {
        Tick previousTick;
        Tick currentTick;
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
    };

    struct RemoveEvent
    {
        EventState eventState;
    };

    struct RemoveEventByIndex
    {
        EventIndex eventIndex;
    };

    struct RemoveDontDeleteFlag
    {
        EventIndex eventIndex;
    };

    struct UpdateEventTick
    {
        EventState eventState;
        Tick newTick;
    };

    struct RemoveDoubles
    {
    };

    struct AddToEventIndex
    {
        int valueToAdd;
    };

    struct UpdateEvent
    {
        EventState eventState;
    };

    using TrackEventMessage =
        std::variant<SyncEventIndex, InsertEvent, ClearEvents, RemoveEvent,
                     RemoveEventByIndex, RemoveDontDeleteFlag, UpdateEventTick,
                     RemoveDoubles, AddToEventIndex,
                     UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent>;
} // namespace mpc::sequencer
