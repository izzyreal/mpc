#pragma once

#include "MpcSpecs.hpp"

#include "IntTypes.hpp"

#include <array>
#include <cstdint>

namespace mpc::sequencer
{
    struct SongStepState
    {
        SequenceIndex sequenceIndex{NoSequenceIndex};
        int8_t repetitionCount{1};
    };

    struct SongState
    {
        bool used;
        std::array<SongStepState, Mpc2000XlSpecs::MAX_SONG_STEP_COUNT> steps;
        SongStepIndex firstLoopStepIndex;
        SongStepIndex lastLoopStepIndex;
        bool loopEnabled = false;

        SongState()
        {
            initializeDefaults();
        }

        void initializeDefaults()
        {
            used = false;
            steps = {};
            firstLoopStepIndex = MinSongStepIndex;
            lastLoopStepIndex = MinSongStepIndex;
            loopEnabled = false;
        }
    };
} // namespace mpc::sequencer