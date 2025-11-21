#pragma once

#include "IntTypes.hpp"

#include <memory>
#include <cstdint>

namespace mpc::sequencer
{
    struct SequenceState;

    class SequenceStateView
    {
    public:
        explicit SequenceStateView(
            const std::shared_ptr<const SequenceState> &s) noexcept;

    private:
        const std::shared_ptr<const SequenceState> state;
    };
} // namespace mpc::sequencer
