#pragma once

#include "sequencer/TrackEventState.hpp"
#include "sequencer/TrackEventStateView.hpp"
#include "sequencer/TrackEventMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class TrackEvent;

    class TrackEventStateManager final
        : public concurrency::AtomicStateExchange<
              TrackEventState, TrackEventStateView, TrackEventMessage>
    {
    public:
        explicit TrackEventStateManager();

    protected:
        void applyMessage(const TrackEventMessage &msg) noexcept override;
    };
} // namespace mpc::sequencer
