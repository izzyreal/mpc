#pragma once

#include "IntTypes.hpp"

#include <memory>
#include <cstdint>

namespace mpc::sequencer
{
    struct TrackEventState;

    class TrackEventStateView
    {
    public:
        explicit TrackEventStateView(
            const std::shared_ptr<const TrackEventState> &s) noexcept;

    private:
        const std::shared_ptr<const TrackEventState> state;
    };
} // namespace mpc::sequencer
