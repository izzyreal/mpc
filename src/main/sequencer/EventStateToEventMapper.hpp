#pragma once

#include "sequencer/TrackEventMessage.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class Event;

    std::shared_ptr<Event> mapEventStateToEvent(
        const EventState &,
        const std::function<void(TrackEventMessage &&)> &dispatch);
} // namespace mpc::sequencer