#pragma once

#include "sequencer/NoteRange.hpp"
#include "sequencer/PlaybackState.hpp"

#include "sequencer/TransportMessage.hpp"
#include "sequencer/SequenceMessage.hpp"

#include <functional>
#include <variant>

namespace mpc::sequencer
{
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

    using SequencerMessage =
        std::variant<TransportMessage, SequenceMessage, UpdatePlaybackState,
                     RefreshPlaybackStateWhileNotPlaying,
                     RefreshPlaybackStateWhilePlaying, CopyEvents,
                     SwitchToNextSequence, SetSelectedSequenceIndex>;

    using MessagesThatInvalidatePlaybackStateWhileNotPlaying =
        std::variant<SetSelectedSequenceIndex>;

    using MessagesThatInvalidatePlaybackStateWhilePlaying =
        std::variant<SwitchToNextSequence>;

} // namespace mpc::sequencer
