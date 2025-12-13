#pragma once

#include "sequencer/TrackMessage.hpp"
#include "sequencer/EventData.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class SequencerStateManager;
    class EventRef;
    class Sequence;

    std::shared_ptr<EventRef>
    mapEventDataToEvent(EventData *ptr, const EventData &snapshot,
                        const std::function<void(TrackMessage &&)> &dispatch,
                        Sequence *parent);
} // namespace mpc::sequencer