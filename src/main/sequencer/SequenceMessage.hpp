#pragma once

#include "sequencer/TimeSignature.hpp"
#include "sequencer/TrackMessage.hpp"

#include "utils/SmallFn.hpp"

#include <variant>

namespace mpc::sequencer
{
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
        utils::SimpleAction nextAction{[]{}};
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
                     UpdateBarLengths, UpdateTimeSignatures,
                     SetTimeSignature, SetLoopEnabled, SetSequenceUsed,
                     SetTempoChangeEnabled, SetFirstLoopBarIndex,
                     SetLastLoopBarIndex, SyncTrackEventIndices, MoveTrack,
                     DeleteTrack, DeleteAllTracks>;
} // namespace mpc::sequencer
