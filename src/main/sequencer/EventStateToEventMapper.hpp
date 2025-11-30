#pragma once

#include "SequencerStateView.hpp"
#include "sequencer/SequenceMessage.hpp"
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
        const std::function<void(SequenceMessage &&)> &dispatch,
        Sequence *parent);
} // namespace mpc::sequencer