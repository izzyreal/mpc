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

    struct StopSequence
    {
    };

    struct PlaySequence
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

    struct PlaySequenceFromStart
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

    struct PlaySong
    {
    };

    struct PlaySongFromStart
    {
    };

    struct StopSong
    {
    };

    struct SetEndOfSongEnabled
    {
        bool endOfSongEnabled;
    };

    struct SetPlayedSongStepRepetitionCount
    {
        int count;
    };

    struct SetCountInPositions
    {
        Tick start;
        Tick end;
    };

    struct SetMasterTempo
    {
        double tempo;
    };

    struct SetTempoSourceIsSequence
    {
        bool enabled;
    };

    struct SetShouldWaitForMidiClockLock
    {
        bool enabled;
    };

    struct SetPositionTicksToReturnToWhenReleaseRec
    {
        Tick pos;
    };

    using TransportMessage =
        std::variant<SetPositionQuarterNotes, StopSequence, PlaySequence,
                     PlaySequenceFromStart, Record, RecordFromStart, Overdub,
                     OverdubFromStart, SwitchRecordToOverdub,
                     SetRecordingEnabled, SetOverdubbingEnabled,
                     SetCountEnabled, SetCountingIn, SetMetronomeOnlyEnabled,
                     SetMetronomeOnlyTickPosition,
                     BumpMetronomeOnlyTickPositionOneTick, PlayMetronomeOnly,
                     StopMetronomeOnly, PlaySong, PlaySongFromStart, StopSong,
                     SetCountInPositions, SetEndOfSongEnabled,
                     SetPlayedSongStepRepetitionCount, SetMasterTempo,
                     SetTempoSourceIsSequence, SetShouldWaitForMidiClockLock,
                     SetPositionTicksToReturnToWhenReleaseRec>;
} // namespace mpc::sequencer
