#pragma once

#include "IntTypes.hpp"

#include <variant>
#include <cstdint>

namespace mpc::sequencer
{
    struct SetSongStepSequenceIndex
    {
        SongIndex songIndex;
        SongStepIndex stepIndex;
        SequenceIndex sequenceIndex;
    };

    struct SetSongStepRepetitionCount
    {
        SongIndex songIndex;
        SongStepIndex stepIndex;
        int8_t count;
    };

    struct SetSongLoopEnabled
    {
        SongIndex songIndex;
        bool enabled;
    };

    struct SetSongFirstLoopStepIndex
    {
        SongIndex songIndex;
        SongStepIndex stepIndex;
    };

    struct SetSongLastLoopStepIndex
    {
        SongIndex songIndex;
        SongStepIndex stepIndex;
    };

    struct DeleteSongStep
    {
        SongIndex songIndex;
        SongStepIndex stepIndex;
    };

    struct InsertSongStep
    {
        SongIndex songIndex;
        SongStepIndex stepIndex;
    };

    struct SetSongUsed
    {
        SongIndex songIndex;
        bool used;
    };

    struct DeleteSong
    {
        SongIndex songIndex;
    };

    using SongMessage =
        std::variant<SetSongStepSequenceIndex, SetSongStepRepetitionCount,
                     SetSongLoopEnabled, SetSongFirstLoopStepIndex,
                     SetSongLastLoopStepIndex, DeleteSongStep, InsertSongStep,
                     SetSongUsed, DeleteSong>;
} // namespace mpc::sequencer