#pragma once

#include "sequencer/TimeSignature.hpp"
#include "sequencer/EventState.hpp"

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
        std::vector<EventState> eventStates;
    };

    struct UpdateTrackIndexOfAllEvents
    {
        SequenceIndex sequence;
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
        std::function<void()> onComplete = [] {};
    };

    struct RemoveEvent
    {
        SequenceIndex sequence;
        TrackIndex track;
        EventId eventId;
    };

    struct UpdateEventTick
    {
        EventState eventState;
        Tick newTick;
    };

    struct RemoveDoubles
    {
        SequenceIndex sequence;
        TrackIndex track;
    };

    struct UpdateEvent
    {
        EventState payload;
    };

    struct UpdateBarLength
    {
        SequenceIndex sequenceIndex;
        int barIndex;
        Tick length;
    };

    struct UpdateBarLengths
    {
        SequenceIndex sequenceIndex;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
    };

    struct UpdateTimeSignatures
    {
        SequenceIndex sequenceIndex;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures;
    };

    struct UpdateTimeSignature
    {
        SequenceIndex sequenceIndex;
        int barIndex;
        TimeSignature timeSignature;
    };

    struct UpdateSequenceEvents
    {
        SequenceIndex sequenceIndex;
        std::vector<EventState> eventStates;
    };

    struct SetLastBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    struct InsertBars
    {
        SequenceIndex sequenceIndex;
        int barCount;
        BarIndex afterBar;
        std::function<void(BarIndex newLastBarIndex)> onComplete =
            [](const BarIndex) {};
    };

    struct SetInitialTempo
    {
        SequenceIndex sequenceIndex;
        double initialTempo;
    };

    struct SetLoopEnabled
    {
        SequenceIndex sequenceIndex;
        bool loopEnabled;
    };

    struct SetUsed
    {
        SequenceIndex sequenceIndex;
        bool used;
    };

    struct SetTempoChangeEnabled
    {
        SequenceIndex sequenceIndex;
        bool tempoChangeEnabled;
    };

    struct SetFirstLoopBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    struct SetLastLoopBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    using SequenceMessage = std::variant<
        InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick, RemoveDoubles,
        SetLastBarIndex, InsertBars, UpdateTrackIndexOfAllEvents, UpdateEvent,
        SetInitialTempo, FinalizeNonLiveNoteEvent, UpdateBarLength,
        UpdateBarLengths, UpdateTimeSignatures, UpdateTimeSignature,
        UpdateEvents, UpdateSequenceEvents, SetLoopEnabled, SetUsed,
        SetTempoChangeEnabled, SetFirstLoopBarIndex, SetLastLoopBarIndex>;

    using SequenceMessagesThatInvalidatePlaybackStateWhileNotPlaying =
        std::variant<InsertEvent, /*ClearEvents, */ RemoveEvent,
                     UpdateEventTick, RemoveDoubles,
                     UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent, SetLastBarIndex, InsertBars,
                     UpdateBarLength, UpdateBarLengths, UpdateTimeSignatures,
                     UpdateTimeSignature, UpdateEvents, UpdateSequenceEvents,
                     SetLoopEnabled, SetUsed, SetTempoChangeEnabled, SetInitialTempo,
                     SetFirstLoopBarIndex, SetLastLoopBarIndex>;

    using SequenceMessagesThatInvalidatePlaybackStateWhilePlaying =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     SetInitialTempo,
                     FinalizeNonLiveNoteEvent, UpdateTimeSignature>;

} // namespace mpc::sequencer
