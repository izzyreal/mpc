#pragma once

#include "sequencer/EventData.hpp"
#include "sequencer/BusType.hpp"

#include <functional>
#include <variant>

namespace mpc::sequencer
{
    struct ClearEvents
    {
        SequenceIndex sequence;
        TrackIndex track;
    };

    struct UpdateEvents
    {
        SequenceIndex sequence;
        TrackIndex track;
        std::vector<EventData> snapshot;
    };

    struct UpdateTrackIndexOfAllEvents
    {
        SequenceIndex sequence;
        TrackIndex trackIndex;
    };

    struct FinalizeNonLiveNoteEvent
    {
        EventData *handle;
        Duration duration;
    };

    struct InsertEvent
    {
        EventData *handle;
        bool allowMultipleNoteEventsWithSameNoteOnSameTick;
        std::function<void()> onComplete = [] {};
    };

    struct RemoveEvent
    {
        SequenceIndex sequence;
        TrackIndex track;
        EventData *handle;
    };

    struct UpdateEventTick
    {
        EventData *handle;
        Tick newTick;
    };

    struct RemoveDoubles
    {
        SequenceIndex sequence;
        TrackIndex track;
    };

    struct UpdateEvent
    {
        EventData *event;
        EventData snapshot;
    };

    struct SetTrackUsed
    {
        SequenceIndex sequence;
        TrackIndex track;
        bool used;
    };

    struct SetTrackOn
    {
        SequenceIndex sequence;
        TrackIndex track;
        bool on;
    };

    struct SetPlayEventIndex
    {
        SequenceIndex sequence;
        TrackIndex track;
        EventIndex idx;
    };

    struct SetTrackVelocityRatio
    {
        SequenceIndex sequence;
        TrackIndex track;
        uint8_t ratio;
    };

    struct SetTrackProgramChange
    {
        SequenceIndex sequence;
        TrackIndex track;
        uint8_t pgm;
    };

    struct SetTrackBusType
    {
        SequenceIndex sequence;
        TrackIndex track;
        BusType busType;
    };

    struct SetTrackDeviceIndex
    {
        SequenceIndex sequence;
        TrackIndex track;
        uint8_t deviceIndex;
    };

    using TrackMessage =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent, UpdateEvents, SetTrackUsed,
                     SetTrackOn, SetPlayEventIndex, SetTrackVelocityRatio,
                     SetTrackProgramChange, SetTrackBusType, SetTrackDeviceIndex>;
} // namespace mpc::sequencer
