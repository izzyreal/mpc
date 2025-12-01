#pragma once

#include "IntTypes.hpp"

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

    struct SetRecordingEnabled
    {
        bool recording;
    };
    struct SetOverdubbingEnabled
    {
        bool overdubbing;
    };

    struct SetCountEnabled
    {
        bool enabled;
    };

    struct SetMetronomeOnlyEnabled
    {
        bool enabled;
    };

    struct PlayMetronomeOnly
    {
    };

    struct StopMetronomeOnly
    {
    };

    struct SetMetronomeOnlyTickPosition
    {
        Tick position;
    };

    struct BumpMetronomeOnlyTickPositionOneTick
    {
    };

    using TransportMessage =
        std::variant<SetPositionQuarterNotes, Stop, Play, PlayFromStart, Record,
                     RecordFromStart, Overdub, OverdubFromStart,
                     SwitchRecordToOverdub, SetRecordingEnabled,
                     SetOverdubbingEnabled, SetCountEnabled, SetCountingIn,
                     SetMetronomeOnlyEnabled, SetMetronomeOnlyTickPosition,
                     BumpMetronomeOnlyTickPositionOneTick, PlayMetronomeOnly,
                     StopMetronomeOnly>;
} // namespace mpc::sequencer
