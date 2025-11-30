#pragma once

#include "sequencer/TrackMessage.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class SequencerStateManager;
    class Event;
    class Sequence;

    std::shared_ptr<Event> mapEventStateToEvent(
        EventState *,
        const std::function<void(TrackMessage &&)> &dispatch,
        Sequence *parent);
} // namespace mpc::sequencer