#pragma once

#include "sequencer/TrackEventMessage.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class Event;
    class Sequence;

    std::shared_ptr<Event> mapEventStateToEvent(
        const EventState &,
        const std::function<void(TrackEventMessage &&)> &dispatch,
        EventIndex eventIndex, Sequence *parent);
} // namespace mpc::sequencer