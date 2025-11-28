#pragma once

#include "IntTypes.hpp"

#include <memory>

namespace mpc::sequencer
{
    struct SequencerAudioState;

    class SequencerAudioStateView
    {
    public:
        explicit SequencerAudioStateView(
            const std::shared_ptr<const SequencerAudioState> &s) noexcept;

        TimeInSamples getTimeInSamples() const noexcept;

    private:
        const std::shared_ptr<const SequencerAudioState> state;
    };
} // namespace mpc::sequencer
