#pragma once

#include "IntTypes.hpp"
#include "TimeSignature.hpp"

#include <memory>

namespace mpc::sequencer
{
    struct SequenceState;

    class SequenceStateView
    {
    public:
        explicit SequenceStateView(
            const std::shared_ptr<const SequenceState> &s) noexcept;

        Tick getBarLength(int barIndex) const;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> getBarLengths() const;

        TimeSignature getTimeSignature(int barIndex) const;

    private:
        const std::shared_ptr<const SequenceState> state;
    };
} // namespace mpc::sequencer
