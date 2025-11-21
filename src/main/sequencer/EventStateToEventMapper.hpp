#pragma once

#include <memory>

namespace mpc::sequencer
{
    class EventState;
    class Event;

    std::shared_ptr<Event> mapEventStateToEvent(const EventState &);
}