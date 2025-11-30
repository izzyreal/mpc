#pragma once

#include "SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class SequencerStateManager;
    class Event;
    class Sequence;

    std::shared_ptr<Event> mapEventStateToEvent(
        std::function<std::shared_ptr<TrackStateView>()> getTrackSnapshot,
        const EventState &,
        const std::function<void(SequencerMessage &&)> &dispatch,
        Sequence *parent);
} // namespace mpc::sequencer