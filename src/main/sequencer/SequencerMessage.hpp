#pragma once

#include "IntTypes.hpp"

#include <cstdint>
#include <variant>
#include <functional>

namespace mpc::sequencer
{
    struct SetTimeInSamples
    {
        TimeInSamples timeInSamples;
    };

    struct SetPositionQuarterNotes
    {
        double positionQuarterNotes;
        std::function<void()> onComplete = []{};
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

    using SequencerMessage =
        std::variant<SetTimeInSamples, SetPositionQuarterNotes, SetPlayStartPositionQuarterNotes,
                     BumpPositionByTicks, SwitchToNextSequence,
                     SetSelectedSequenceIndex, Stop, Play>;
} // namespace mpc::sequencer
