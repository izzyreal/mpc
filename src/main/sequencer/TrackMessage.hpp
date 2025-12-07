#pragma once

#include "sequencer/EventData.hpp"
#include "sequencer/BusType.hpp"
#include "utils/SimpleAction.hpp"

#include <variant>
#include <vector>

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

    struct FinalizeNoteEventNonLive
    {
        EventData *handle;
        Duration duration;
    };

    struct InsertAcquiredEvent
    {
        EventData *handle;
        utils::SimpleAction onComplete{[] {}};
    };

    struct RemoveEvent
    {
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
        EventData *handle;
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

    struct SetTrackTransmitProgramChangesEnabled
    {
        SequenceIndex sequenceIndex;
        TrackIndex trackIndex;
        bool enabled;
    };

    using TrackMessage = std::variant<
        InsertAcquiredEvent, ClearEvents, RemoveEvent, UpdateEventTick,
        RemoveDoubles, UpdateEvent, FinalizeNoteEventNonLive, UpdateEvents,
        SetTrackUsed, SetTrackOn, SetPlayEventIndex, SetTrackVelocityRatio,
        SetTrackProgramChange, SetTrackBusType, SetTrackDeviceIndex,
        SetTrackTransmitProgramChangesEnabled>;
} // namespace mpc::sequencer
