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
        double ticks;
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
        std::variant<SetPositionQuarterNotes, SetPlayStartPositionQuarterNotes,
                     BumpPositionByTicks, Stop, Play, PlayFromStart, Record,
                     RecordFromStart, Overdub, OverdubFromStart,
                     SwitchRecordToOverdub, UpdateRecording, UpdateOverdubbing,
                     UpdateCountEnabled>;

    using TransportMessagesThatInvalidatePlaybackStateWhileNotPlaying =
        std::variant<BumpPositionByTicks, UpdateCountEnabled>;

    using TransportMessagesThatInvalidatePlaybackStateWhilePlaying =
        std::variant<UpdateCountEnabled>;

} // namespace mpc::sequencer
