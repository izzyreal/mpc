#pragma once

#include "IntTypes.hpp"

#include <memory>
#include <cstdint>

namespace mpc::sequencer
{
    struct SequencerState;

    class SequencerStateView
    {
    public:
        explicit SequencerStateView(
            const std::shared_ptr<const SequencerState> &s) noexcept;

        TimeInSamples getTimeInSamples() const noexcept;

    private:
        const std::shared_ptr<const SequencerState> state;
    };
} // namespace mpc::sequencer
