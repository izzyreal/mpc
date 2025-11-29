#pragma once

#include <variant>

namespace mpc::sequencer
{
    struct SetPositionQuarterNotes
    {
        double positionQuarterNotes;
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
                     UpdateCountEnabled>;

    using TransportMessagesThatInvalidatePlaybackStateWhileNotPlaying =
        std::variant<UpdateCountEnabled>;

    using TransportMessagesThatInvalidatePlaybackStateWhilePlaying =
        std::variant<UpdateCountEnabled>;

} // namespace mpc::sequencer
