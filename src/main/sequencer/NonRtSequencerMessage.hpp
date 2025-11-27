#pragma once

#include "PlaybackState.hpp"
#include "TimeSignature.hpp"
#include "sequencer/EventState.hpp"

#include <cstdint>
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
        std::function<void()> onComplete;
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

    struct SetPositionQuarterNotes
    {
        double positionQuarterNotes;
    };

    struct SetPlayStartPositionQuarterNotes
    {
        double positionQuarterNotes;
    };

    struct BumpPositionByTicks
    {
        uint8_t ticks;
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

    struct Stop
    {
    };

    struct Play
    {
    };

    struct Record
    {
    };

    struct RecordFromStart
    {
    };

    struct Overdub
    {
    };

    struct OverdubFromStart
    {
    };

    struct SwitchRecordToOverdub
    {
    };

    struct PlayFromStart
    {
    };

    struct UpdateRecording { bool recording; };
    struct UpdateOverdubbing { bool overdubbing; };

    struct UpdateCountEnabled { bool enabled; };

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

    using NonRtSequencerMessage =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent, UpdatePlaybackState,
                     RefreshPlaybackStateWhileNotPlaying, RefreshPlaybackStateWhilePlaying,
                     SetPositionQuarterNotes, SetPlayStartPositionQuarterNotes,
                     BumpPositionByTicks, SwitchToNextSequence,
                     SetSelectedSequenceIndex, Stop, Play, PlayFromStart,
                     Record, RecordFromStart, Overdub, OverdubFromStart, SwitchRecordToOverdub,
                     UpdateRecording, UpdateOverdubbing, UpdateCountEnabled,
                     UpdateBarLength, UpdateBarLengths, UpdateTimeSignatures,
                     UpdateTimeSignature, UpdateEvents, UpdateSequenceEvents>;

    using MessagesThatInvalidPlaybackStateWhileNotPlaying =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent, SetPositionQuarterNotes,
                     SetPlayStartPositionQuarterNotes, BumpPositionByTicks,
                     SetSelectedSequenceIndex,
                     UpdateBarLength, UpdateBarLengths, UpdateTimeSignatures,
                     UpdateCountEnabled,
                     UpdateTimeSignature, UpdateEvents, UpdateSequenceEvents>;

    using MessagesThatInvalidPlaybackStateWhilePlaying =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent,
                     SwitchToNextSequence, UpdateCountEnabled,
                     UpdateTimeSignature>;

} // namespace mpc::sequencer
