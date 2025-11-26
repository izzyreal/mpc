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
        bool fromStart;
    };

    struct RequestRefreshPlaybackState
    {
        TimeInSamples timeInSamples;
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

    using NonRtSequencerMessage =
        std::variant<InsertEvent, ClearEvents, RemoveEvent, UpdateEventTick,
                     RemoveDoubles, UpdateTrackIndexOfAllEvents, UpdateEvent,
                     FinalizeNonLiveNoteEvent, UpdatePlaybackState,
                     RequestRefreshPlaybackState, SetPositionQuarterNotes,
                     SetPlayStartPositionQuarterNotes, BumpPositionByTicks,
                     SwitchToNextSequence, SetSelectedSequenceIndex, Stop,
                     Play, UpdateBarLength, UpdateBarLengths, UpdateTimeSignatures, UpdateTimeSignature,
                     UpdateEvents>;
} // namespace mpc::sequencer
