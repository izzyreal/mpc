#pragma once

#include "sequencer/TimeSignature.hpp"
#include "sequencer/TrackMessage.hpp"

#include <functional>
#include <variant>

namespace mpc::sequencer
{
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
    };

    struct RemoveEventsThatAreOutsideTickBounds
    {
        SequenceIndex sequenceIndex;
    };

    using SequenceMessage =
        std::variant<TrackMessage, SetLastBarIndex, InsertBars, SetInitialTempo,
                     UpdateBarLength, UpdateBarLengths, UpdateTimeSignatures,
                     SetTimeSignature, SetLoopEnabled, SetSequenceUsed,
                     SetTempoChangeEnabled, SetFirstLoopBarIndex,
                     SetLastLoopBarIndex, SyncTrackEventIndices,
                     RemoveEventsThatAreOutsideTickBounds>;
} // namespace mpc::sequencer
