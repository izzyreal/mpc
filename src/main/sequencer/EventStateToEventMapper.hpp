#pragma once

#include "NonRtSequencerStateView.hpp"
#include "sequencer/NonRtSequencerMessage.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>

namespace mpc::sequencer
{
    class NonRtSequencerStateManager;
    class Event;
    class Sequence;

    std::shared_ptr<Event> mapEventStateToEvent(
        std::function<std::shared_ptr<NonRtTrackStateView>()> getTrackSnapshot, const EventState &,
        const std::function<void(NonRtSequencerMessage &&)> &dispatch,
        Sequence *parent);
} // namespace mpc::sequencer