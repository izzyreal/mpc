#pragma once

#include "sequencer/TrackEventMessage.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class TrackEventStateManager;
    class Event;
    class Sequence;

    std::shared_ptr<Event> mapEventStateToEvent(
        std::shared_ptr<TrackEventStateManager>,
        const EventState &,
        const std::function<void(TrackEventMessage &&)> &dispatch,
        Sequence *parent);
} // namespace mpc::sequencer