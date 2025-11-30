#pragma once

#include <variant>

namespace mpc::sequencer
{
    struct SetPositionQuarterNotes
    {
        double positionQuarterNotes;
    };

    struct SetCountingIn
    {
        bool countingIn;
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

    struct UpdateRecording
    {
        bool recording;
    };
    struct UpdateOverdubbing
    {
        bool overdubbing;
    };

    struct UpdateCountEnabled
    {
        bool enabled;
    };

    using TransportMessage =
        std::variant<SetPositionQuarterNotes,
                     Stop, Play, PlayFromStart, Record,
                     RecordFromStart, Overdub, OverdubFromStart,
                     SwitchRecordToOverdub, UpdateRecording, UpdateOverdubbing,
                     UpdateCountEnabled, SetCountingIn>;
} // namespace mpc::sequencer
