#pragma once

#include <memory>
#include <cstdint>

namespace mpc::sequencer
{
    class SequencerState;

    class SequencerStateView
    {
    public:
        explicit SequencerStateView(const std::shared_ptr<const SequencerState> s) noexcept;

        double getPositionQuarterNotes() const;

        int64_t getPositionTicks() const;

    private:
        const std::shared_ptr<const SequencerState> state;

    };
}
