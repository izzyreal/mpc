#pragma once

#include "sequencer/SongState.hpp"

namespace mpc::sequencer
{
    class SongStateView
    {
    public:
        explicit SongStateView(const SongState &s) noexcept;

        bool isLoopEnabled() const;

        SongStepIndex getFirstLoopStepIndex() const;

        SongStepIndex getLastLoopStepIndex() const;

        bool isUsed() const;

        SongStepState getStep(SongStepIndex) const;

        uint8_t getStepCount();

    private:
        const SongState state;
    };
} // namespace mpc::sequencer