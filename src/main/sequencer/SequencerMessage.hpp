#pragma once

#include <cstdint>
#include <variant>

namespace mpc::sequencer
{
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

    struct SetActiveSequenceIndex
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

    using SequencerMessage =
        std::variant<SetPositionQuarterNotes, SetPlayStartPositionQuarterNotes,
                     BumpPositionByTicks, SwitchToNextSequence,
                     SetActiveSequenceIndex, Stop, Play>;
} // namespace mpc::sequencer
