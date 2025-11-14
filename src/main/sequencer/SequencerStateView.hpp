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

        SequenceIndex getSelectedSequenceIndex() const noexcept;

        double getPositionQuarterNotes() const;

        double getPlayStartPositionQuarterNotes() const;

        int64_t getPositionTicks() const;

    private:
        const std::shared_ptr<const SequencerState> state;
    };
} // namespace mpc::sequencer
