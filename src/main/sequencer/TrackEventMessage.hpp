#pragma once

#include "IntTypes.hpp"

#include <cstdint>
#include <variant>

namespace mpc::sequencer
{
    struct UpdateTrackEvent
    {
    };

    using TrackEventMessage =
        std::variant<UpdateTrackEvent>;
} // namespace mpc::sequencer
