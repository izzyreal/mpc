#pragma once

#include "sequencer/TimeSignature.hpp"
#include "sequencer/TrackMessage.hpp"

#include "utils/SmallFn.hpp"

#include <variant>

namespace mpc::sequencer
{
    struct SetTimeSignature
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
        TimeSignature timeSignature;
    };

    struct SetLastBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    struct InsertBars
    {
        using Callback = utils::SmallFn<64, void(BarIndex)>;
        SequenceIndex sequenceIndex;
        int barCount;
        BarIndex afterBar;
        Callback onComplete{[](BarIndex) {}};
        utils::SimpleAction nextAction{[] {}};
    };

    struct DeleteBars
    {
        SequenceIndex sequenceIndex;
        BarIndex firstBarIndex; // inclusive
        BarIndex lastBarIndex; // inclusive
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

    struct SetSequenceUsed
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

    struct SyncTrackEventIndices
    {
        SequenceIndex sequenceIndex;
    };

    struct MoveTrack
    {
        SequenceIndex sequenceIndex;
        TrackIndex source;
        TrackIndex destination;
    };

    struct DeleteTrack
    {
        SequenceIndex sequenceIndex;
        TrackIndex trackIndex;
    };

    struct DeleteAllTracks
    {
        SequenceIndex sequenceIndex;
    };

    using SequenceMessage =
        std::variant<TrackMessage, SetLastBarIndex, InsertBars, SetInitialTempo,
                     SetTimeSignature, SetLoopEnabled, SetSequenceUsed,
                     SetTempoChangeEnabled, SetFirstLoopBarIndex,
                     SetLastLoopBarIndex, SyncTrackEventIndices, MoveTrack,
                     DeleteTrack, DeleteAllTracks, DeleteBars>;
} // namespace mpc::sequencer
