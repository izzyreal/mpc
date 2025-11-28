#pragma once

#include "sequencer/NoteRange.hpp"
#include "sequencer/PlaybackState.hpp"
#include "sequencer/TimeSignature.hpp"
#include "sequencer/EventState.hpp"
#include "sequencer/TransportMessage.hpp"

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

    struct UpdatePlaybackState
    {
        PlaybackState playbackState;
        std::function<void()> onComplete = [] {};
    };

    struct SwitchToNextSequence
    {
        SequenceIndex sequenceIndex;
    };

    struct SetSelectedSequenceIndex
    {
        SequenceIndex sequenceIndex;
        bool setPositionTo0 = true;
    };

    struct RefreshPlaybackStateWhileNotPlaying
    {
        std::function<void()> onComplete = [] {};
    };

    struct RefreshPlaybackStateWhilePlaying
    {
        std::function<void()> onComplete = [] {};
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

    struct CopyEvents
    {
        Tick sourceStartTick;
        Tick sourceEndTick;
        SequenceIndex sourceSequenceIndex;
        SequenceIndex destSequenceIndex;
        TrackIndex sourceTrackIndex;
        TrackIndex destTrackIndex;
        Tick destStartTick;
        bool copyModeMerge;
        int copyCount;
        NoteRange sourceNoteRange{};
        std::function<EventId()> generateEventId;
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

    using SequencerMessage = std::variant<
        TransportMessage,
        InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick, RemoveDoubles,
        CopyEvents, SetLastBarIndex, InsertBars, UpdateTrackIndexOfAllEvents,
        UpdateEvent, SetInitialTempo, FinalizeNonLiveNoteEvent,
        UpdatePlaybackState, RefreshPlaybackStateWhileNotPlaying,
        RefreshPlaybackStateWhilePlaying,
        SwitchToNextSequence, SetSelectedSequenceIndex, UpdateBarLength, UpdateBarLengths,
        UpdateTimeSignatures, UpdateTimeSignature, UpdateEvents,
        UpdateSequenceEvents, SetLoopEnabled, SetUsed, SetTempoChangeEnabled,
        SetFirstLoopBarIndex, SetLastLoopBarIndex>;

    using MessagesThatInvalidatePlaybackStateWhileNotPlaying = std::variant<
        TransportMessagesThatInvalidatePlaybackStateWhileNotPlaying,
        InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick, RemoveDoubles,
        UpdateTrackIndexOfAllEvents, UpdateEvent, FinalizeNonLiveNoteEvent,
        SetSelectedSequenceIndex, SetLastBarIndex,
        InsertBars, UpdateBarLength, UpdateBarLengths, UpdateTimeSignatures,
        UpdateTimeSignature, UpdateEvents,
        UpdateSequenceEvents, SetLoopEnabled, SetUsed, SetTempoChangeEnabled,
        SetFirstLoopBarIndex, SetLastLoopBarIndex>;

    using MessagesThatInvalidatePlaybackStateWhilePlaying =
        std::variant<
            TransportMessagesThatInvalidatePlaybackStateWhilePlaying,
            InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent, SwitchToNextSequence,
                     UpdateTimeSignature>;

} // namespace mpc::sequencer
